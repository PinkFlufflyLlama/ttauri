// Copyright 2019 Pokitec
// All rights reserved.

#include "TrueTypeParser.hpp"
#include "BezierPoint.hpp"
#include "Path.hpp"
#include "Font.hpp"
#include "TTauri/exceptions.hpp"
#include "TTauri/FileView.hpp"
#include <boost/endian/buffers.hpp>
#include <boost/format.hpp>
#include <boost/exception/all.hpp>
#include <map>
#include <array>

using namespace boost::endian;

namespace TTauri::Draw {


struct Fixed_buf_t {
    big_uint32_buf_t x;
    float value() const { return static_cast<float>(x.value()) / 65536.0f; }
};

struct shortFrac_buf_t {
    big_int16_buf_t x;
    float value() const { return static_cast<float>(x.value()) / 32768.0f; }
};

struct FWord_buf_t {
    big_int16_buf_t x;
    float value(uint16_t unitsPerEm) const { return static_cast<float>(x.value()) / static_cast<float>(unitsPerEm); }
};

struct FByte_buf_t {
    int8_t x;
    float value(uint16_t unitsPerEm) const { return static_cast<float>(x) / static_cast<float>(unitsPerEm); }
};

struct uFWord_buf_t {
    big_uint16_buf_t x;
    float value(uint16_t unitsPerEm) const { return static_cast<float>(x.value()) / static_cast<float>(unitsPerEm); }
};

struct SFNTHeader {
    big_uint32_buf_t scalerType;
    big_uint16_buf_t numTables;
    big_uint16_buf_t searchRange;
    big_uint16_buf_t entrySelector;
    big_uint16_buf_t rangeShift;
};

struct SFNTEntry {
    big_uint32_buf_t tag;
    big_uint32_buf_t checkSum;
    big_uint32_buf_t offset;
    big_uint32_buf_t length;
};

struct GLYFEntry {
    big_int16_buf_t numberOfContours;
    FWord_buf_t xMin;
    FWord_buf_t yMin;
    FWord_buf_t xMax;
    FWord_buf_t yMax;
};

struct CMAPHeader {
    big_uint16_buf_t version;
    big_uint16_buf_t numTables;
};

struct CMAPEntry {
    big_uint16_buf_t platformID;
    big_uint16_buf_t platformSpecificID;
    big_uint32_buf_t offset;
};

struct CMAPFormat {
    big_uint16_buf_t format;
};

struct CMAPFormat4 {
    big_uint16_buf_t format;
    big_uint16_buf_t length;
    big_uint16_buf_t language;
    big_uint16_buf_t segCountX2;
    big_uint16_buf_t searchRange;
    big_uint16_buf_t entrySelector;
    big_uint16_buf_t rangeShift;
};

struct CMAPFormat6 {
    big_uint16_buf_t format;
    big_uint16_buf_t length;
    big_uint16_buf_t language;
    big_uint16_buf_t firstCode;
    big_uint16_buf_t entryCount;
};

struct CMAPFormat12 {
    big_uint32_buf_t format;
    big_uint32_buf_t length;
    big_uint32_buf_t language;
    big_uint32_buf_t numGroups;
};

struct CMAPFormat12Group {
    big_uint32_buf_t startCharCode;
    big_uint32_buf_t endCharCode;
    big_uint32_buf_t startGlyphCode;
};

struct HEADTable {
    Fixed_buf_t version;
    Fixed_buf_t fontRevision;
    big_uint32_buf_t checkSumAdjustment;
    big_uint32_buf_t magicNumber;
    big_uint16_buf_t flags;
    big_uint16_buf_t unitsPerEm;
    big_uint64_buf_t created;
    big_uint64_buf_t modified;
    FWord_buf_t xMin;
    FWord_buf_t yMin;
    FWord_buf_t xMax;
    FWord_buf_t yMax;
    big_uint16_buf_t macStyle;
    big_uint16_buf_t lowestRecPPEM;
    big_int16_buf_t fontDirectionHint;
    big_int16_buf_t indexToLocFormat;
    big_int16_buf_t glyphDataFormat;
};

struct HHEATable {
    Fixed_buf_t version;
    FWord_buf_t ascender;
    FWord_buf_t descender;
    FWord_buf_t lineGap;
    uFWord_buf_t advanceWidthMax;
    FWord_buf_t minLeftSideBearing;
    FWord_buf_t minRightSideBearing;
    FWord_buf_t xMaxExtent;
    big_int16_buf_t caretSlopeRise;
    big_int16_buf_t caretSlopRun;
    big_int16_buf_t caretOffset;
    big_int16_buf_t reserved0;
    big_int16_buf_t reserved1;
    big_int16_buf_t reserved2;
    big_int16_buf_t reserved3;
    big_int16_buf_t metricDataFormat;
    big_int16_buf_t numberOfHMetrics;
};

struct HMTXEntry {
    uFWord_buf_t advanceWidth;
    FWord_buf_t leftSideBearing;
};

struct MAXPTable {
    big_uint32_buf_t version;
    big_uint16_buf_t numGlyphs;
    big_uint16_buf_t maxPoints;
    big_uint16_buf_t maxContours;
    big_uint16_buf_t maxComponentPoints;
    big_uint16_buf_t maxComponentContours;
    big_uint16_buf_t maxZones;
    big_uint16_buf_t maxTwilightPoints;
    big_uint16_buf_t maxStorage;
    big_uint16_buf_t maxFunctionDefs;
    big_uint16_buf_t maxInstructionDefs;
    big_uint16_buf_t maxStackElements;
    big_uint16_buf_t maxSizeOfInstructions;
    big_uint16_buf_t maxComponentElements;
    big_uint16_buf_t maxComponentDepth;
};

static Path parseGlyph(std::vector<gsl::span<std::byte const>> const& glyphDataList, size_t i, uint16_t unitsPerEm);

static std::map<char32_t, size_t> parseCMAPFormat4(gsl::span<std::byte const> bytes)
{
    let &entry = at<CMAPFormat4>(bytes, 0);
    let segCount = static_cast<size_t>(entry.segCountX2.value()) / 2;

    let endCodesOffset = sizeof(CMAPFormat4);
    let endCodes = make_span<big_uint16_buf_t>(bytes, endCodesOffset, segCount);

    let startCodesOffset = endCodesOffset + 2*segCount + 2;
    let startCodes = make_span<big_uint16_buf_t>(bytes, startCodesOffset, segCount);

    let idDeltasOffset = startCodesOffset + 2*segCount;
    let idDeltas = make_span<big_uint16_buf_t>(bytes, idDeltasOffset, segCount);

    let idRangeIndiciesOffset = idDeltasOffset + 2 * segCount;
    let idRangeIndices = make_span<big_uint16_buf_t>(bytes, idRangeIndiciesOffset, segCount);

    std::map<char32_t, size_t> characterToGlyph;
    for (size_t segmentIndex = 0; segmentIndex < segCount; segmentIndex++) {
        let startCode = startCodes.at(segmentIndex).value();
        let endCode = endCodes.at(segmentIndex).value();
        let idDelta = idDeltas.at(segmentIndex).value();
        let idRangeIndex = static_cast<size_t>(idRangeIndices.at(segmentIndex).value());
        let idRangeIndexOffset = idRangeIndiciesOffset + 2*segmentIndex;

        for (char32_t c = startCode; c <= endCode; c++) {
            if (idRangeIndex == 0) {
                // Calculation must be done modulo 65536.
                characterToGlyph[c] = (c + idDelta) & 0xffff;
            } else {
                let index = idRangeIndex + idRangeIndexOffset + 2*(static_cast<size_t>(c) - startCode);
                characterToGlyph[c] = at<big_uint16_buf_t>(bytes, index).value();
            }
        }
    }

    return characterToGlyph;
}

static std::map<char32_t, size_t> parseCMAPFormat6(gsl::span<std::byte const> bytes)
{
    let& entry = at<CMAPFormat6 const>(bytes, 0);
    let firstCode = entry.firstCode.value();
    let entryCount = entry.entryCount.value();

    let glyphIndexArrayOffset = sizeof(CMAPFormat6);
    let glyphIndexArray = make_span<big_uint16_buf_t>(bytes, glyphIndexArrayOffset, entryCount);

    std::map<char32_t, size_t> characterToGlyph;
    for (uint16_t entryIndex = 0; entryIndex < entryCount; entryIndex++) {
        characterToGlyph[entryIndex + firstCode] = glyphIndexArray.at(entryIndex).value();
    }
    return characterToGlyph;
}

static std::map<char32_t, size_t> parseCMAPFormat12(gsl::span<std::byte const> bytes)
{
    let& entry = at<CMAPFormat12>(bytes, 0);

    let groups = make_span<CMAPFormat12Group>(bytes, sizeof(CMAPFormat12), entry.numGroups.value());

    std::map<char32_t, size_t> characterToGlyph;
    for (let& group: groups) {
        let startCharCode = group.startCharCode.value();
        let endCharCode = group.endCharCode.value();
        auto glyphCode = group.startGlyphCode.value();
        for (char32_t c = startCharCode; c <= endCharCode; c++) {
            characterToGlyph[c] = glyphCode++;
        }
    }
    return characterToGlyph;
}

static gsl::span<CMAPEntry const>::iterator findBestCMAPEntry(gsl::span<CMAPEntry const> entries)
{
    std::vector<std::pair<uint16_t, uint16_t>> bestPlatforms = {
        {0, 4},
        {0, 0}, {0, 1}, {0, 2}, {0, 3},
        {3, 10}, {3, 1}, {3, 0}
    };

    for (let [ID, specificID] : bestPlatforms) {
        let i = std::find_if(entries.begin(), entries.end(), [ID, specificID](let& x) {
            return x.platformID.value() == ID && x.platformSpecificID.value() == specificID;
        });

        if (i != entries.end()) {
            return i;
        }
    }
    return entries.end();
}

static std::map<char32_t, size_t> parseCMAP(gsl::span<std::byte const> bytes)
{
    let& header = at<CMAPHeader>(bytes, 0);
    if (!(header.version.value() == 0)) {
        BOOST_THROW_EXCEPTION(ParseError("cmap.version is not 0"));
    }

    let entries = make_span<CMAPEntry>(bytes, sizeof(CMAPHeader), header.numTables.value());
    
    let i = findBestCMAPEntry(entries);
    if (i == entries.end()) {
        BOOST_THROW_EXCEPTION(ParseError("Could not find a proper unicode character map"));
    }

    let tableOffset = i->offset.value();
    let tableSpan = bytes.subspan(tableOffset, bytes.size() - tableOffset);

    let table = at<CMAPFormat>(tableSpan, 0);
    switch (table.format.value()) {
    case 4: return parseCMAPFormat4(tableSpan);
    case 6: return parseCMAPFormat6(tableSpan);
    case 12: return parseCMAPFormat12(tableSpan);
    default:
        BOOST_THROW_EXCEPTION(ParseError((boost::format("Unexpected character map format %i") % table.format.value()).str()));
    }
}

constexpr uint8_t FLAG_ON_CURVE = 0x01;
constexpr uint8_t FLAG_X_SHORT = 0x02;
constexpr uint8_t FLAG_Y_SHORT = 0x04;
constexpr uint8_t FLAG_REPEAT = 0x08;
constexpr uint8_t FLAG_X_SAME = 0x10;
constexpr uint8_t FLAG_Y_SAME = 0x20;
static Path parseSimpleGlyph(gsl::span<std::byte const> bytes, uint16_t unitsPerEm)
{
    let scale = 1.0f / static_cast<float>(unitsPerEm);
    size_t offset = 0;
    Path glyph;

    let &entry = at<GLYFEntry>(bytes, offset);
    offset += sizeof(GLYFEntry);

    let numberOfContours = static_cast<size_t>(entry.numberOfContours.value());
    let endPoints = make_span<big_uint16_buf_t>(bytes, offset, numberOfContours);
    offset += numberOfContours * sizeof(uint16_t);

    for (let endPoint: endPoints) {
        glyph.contourEndPoints.push_back(endPoint.value());
    }

    let numberOfPoints = endPoints.at(numberOfContours - 1).value() + 1;

    // Skip over the instructions.
    let instructionLength = at<big_uint16_buf_t>(bytes, offset).value();
    offset += sizeof(uint16_t) + instructionLength * sizeof(uint8_t);

    // Extract all the flags.
    std::vector<uint8_t> flags;
    while (flags.size() < numberOfPoints) {
        let flag = at<uint8_t>(bytes, offset++);
        flags.push_back(flag);

        if (flag & FLAG_REPEAT) {
            let repeat = at<uint8_t>(bytes, offset++);
            for (size_t i = 0; i < repeat; i++) {
                flags.push_back(flag);
            }
        }
    }
    assert(flags.size() == numberOfPoints);

    // Get xCoordinates
    std::vector<int16_t> xCoordinates;
    for (let flag: flags) {
        switch (flag & (FLAG_X_SHORT | FLAG_X_SAME)) {
        case 0: // long-vector, different.
            xCoordinates.push_back(at<big_int16_buf_t>(bytes, offset).value());
            offset += sizeof(int16_t);
            break;
        case FLAG_X_SAME: // Long-vector, same.
            xCoordinates.push_back(0);
            break;
        case FLAG_X_SHORT: // short-vector, negative.
            xCoordinates.push_back(-static_cast<int16_t>(at<uint8_t>(bytes, offset)));
            offset += sizeof(uint8_t);
            break;
        case FLAG_X_SAME | FLAG_X_SHORT: // short-vector, positve.
            xCoordinates.push_back(static_cast<int16_t>(at<uint8_t>(bytes, offset)));
            offset += sizeof(uint8_t);
            break;
        default:
            abort();
        }
    }

    // Get yCoordinates
    std::vector<int16_t> yCoordinates;
    for (let flag: flags) {
        switch (flag & (FLAG_Y_SHORT | FLAG_Y_SAME)) {
        case 0: // long-vector, different.
            yCoordinates.push_back(at<big_int16_buf_t>(bytes, offset).value());
            offset += sizeof(int16_t);
            break;
        case FLAG_Y_SAME: // Long-vector, same.
            yCoordinates.push_back(0);
            break;
        case FLAG_Y_SHORT: // short-vector, negative.
            yCoordinates.push_back(-static_cast<int16_t>(at<uint8_t>(bytes, offset)));
            offset += sizeof(uint8_t);
            break;
        case FLAG_Y_SAME | FLAG_Y_SHORT: // short-vector, positive.
            yCoordinates.push_back(static_cast<int16_t>(at<uint8_t>(bytes, offset)));
            offset += sizeof(uint8_t);
            break;
        default:
            abort();
        }
    }

    // Create absolute points
    int16_t x = 0;
    int16_t y = 0;
    size_t pointNr = 0;
    std::vector<BezierPoint> points;
    for (let flag : flags) {
        x += xCoordinates.at(pointNr);
        y += yCoordinates.at(pointNr);

        let type = (flag & FLAG_ON_CURVE) > 0 ?
            BezierPoint::Type::Anchor :
            BezierPoint::Type::QuadraticControl;

        glyph.points.emplace_back(
            x * scale,
            y * scale,
            type 
        );
        pointNr++;
    }

    return glyph;
}

constexpr uint16_t FLAG_ARG_1_AND_2_ARE_WORDS = 0x0001;
constexpr uint16_t FLAG_ARGS_ARE_XY_VALUES = 0x0002;
constexpr uint16_t FLAG_ROUND_XY_TO_GRID = 0x0004;
constexpr uint16_t FLAG_WE_HAVE_A_SCALE = 0x0008;
constexpr uint16_t FLAG_MORE_COMPONENTS = 0x0020;
constexpr uint16_t FLAG_WE_HAVE_AN_X_AND_Y_SCALE = 0x0040;
constexpr uint16_t FLAG_WE_HAVE_A_TWO_BY_TWO = 0x0080;
constexpr uint16_t FLAG_WE_HAVE_INSTRUCTIONS = 0x0100;
constexpr uint16_t FLAG_USE_MY_METRICS = 0x0200;
constexpr uint16_t FLAG_OVERLAP_COMPOUND = 0x0400;
constexpr uint16_t FLAG_SCALED_COMPONENT_OFFSET = 0x0800;
constexpr uint16_t FLAG_UNSCALED_COMPONENT_OFFSET = 0x1000;
static Path parseCompoundGlyph(std::vector<gsl::span<std::byte const>> const& glyphDataList, size_t i, uint16_t unitsPerEm)
{
    let bytes = glyphDataList.at(i);
    size_t offset = 0;

    let& entry = at<GLYFEntry>(bytes, offset);
    offset += sizeof(GLYFEntry);

    Path glyph;

    uint16_t flags;
    do {
        flags = at<big_uint16_buf_t>(bytes, offset).value();
        offset += sizeof(uint16_t);
        let subGlyphIndex = at<big_uint16_buf_t>(bytes, offset).value();
        offset += sizeof(uint16_t);
        let subGlyph = parseGlyph(glyphDataList, subGlyphIndex, unitsPerEm);

        glm::vec2 subGlyphOffset;
        if (flags & FLAG_ARGS_ARE_XY_VALUES) {
            if (flags & FLAG_ARG_1_AND_2_ARE_WORDS) {
                subGlyphOffset.x = at<FWord_buf_t>(bytes, offset).value(unitsPerEm);
                offset += sizeof(int16_t);
                subGlyphOffset.y = at<FWord_buf_t>(bytes, offset).value(unitsPerEm);
                offset += sizeof(int16_t);
            } else {
                subGlyphOffset.x = at<FByte_buf_t>(bytes, offset).value(unitsPerEm);
                offset += sizeof(int8_t);
                subGlyphOffset.y = at<FByte_buf_t>(bytes, offset).value(unitsPerEm);
                offset += sizeof(int8_t);
            }
        } else {
            size_t pointNr1;
            size_t pointNr2;
            if (flags & FLAG_ARG_1_AND_2_ARE_WORDS) {
                pointNr1 = at<big_uint16_buf_t>(bytes, offset).value();
                offset += sizeof(uint16_t);
                pointNr2 = at<big_uint16_buf_t>(bytes, offset).value();
                offset += sizeof(uint16_t);
            }
            else {
                pointNr1 = at<uint8_t>(bytes, offset);
                offset += sizeof(uint8_t);
                pointNr2 = at<uint8_t>(bytes, offset);
                offset += sizeof(uint8_t);
            }
            abort();
            // XXX Calculate subGlyphOffset
        }

        // Start with an identity matrix.
        auto subGlyphScale = glm::mat2x2(1.0f);
        if (flags & FLAG_WE_HAVE_A_SCALE) {
            subGlyphScale[0][0] = at<shortFrac_buf_t>(bytes, offset).value();
            subGlyphScale[1][1] = subGlyphScale[0][0];
            offset += sizeof(uint16_t);
        } else if (flags & FLAG_WE_HAVE_AN_X_AND_Y_SCALE) {
            subGlyphScale[0][0] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
            subGlyphScale[1][1] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
        } else if (flags & FLAG_WE_HAVE_A_TWO_BY_TWO) {
            subGlyphScale[0][0] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
            subGlyphScale[0][1] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
            subGlyphScale[1][0] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
            subGlyphScale[1][1] = at<shortFrac_buf_t>(bytes, offset).value();
            offset += sizeof(uint16_t);
        }

        if (flags & FLAG_SCALED_COMPONENT_OFFSET) {
            subGlyphOffset = subGlyphOffset * subGlyphScale;
        }

        if (flags & FLAG_USE_MY_METRICS) {
            // XXX implement.
        }
        
        glyph += T2D(subGlyphOffset, subGlyphScale) * subGlyph;

    } while (flags & FLAG_MORE_COMPONENTS);
    // Ignore trailing instructions.

    return glyph;
}

static Path parseGlyph(std::vector<gsl::span<std::byte const>> const &glyphDataList, size_t i, uint16_t unitsPerEm)
{
    let bytes = glyphDataList.at(i);
    if (bytes.size() == 0) {
        // Glyph does not have an outline.
        Path glyph = {};
        return glyph;
    }

    let &entry = at<GLYFEntry>(bytes, 0);
    let numberOfContours = entry.numberOfContours.value();

    Path glyph;
    if (numberOfContours == 0) {
        glyph = {};
    } else if (numberOfContours > 0) {
        glyph = parseSimpleGlyph(bytes, unitsPerEm);
    } else {
        glyph = parseCompoundGlyph(glyphDataList, i, unitsPerEm);
    }

    glm::vec2 const position = { entry.xMin.value(unitsPerEm), entry.yMin.value(unitsPerEm) };
    extent2 const extent = {
        entry.xMax.value(unitsPerEm) - position.x,
        entry.yMax.value(unitsPerEm) - position.y
    };

    glyph.boundingBox = { position, extent };
    return glyph;
}

static std::vector<Path> parseGLYF(std::vector<gsl::span<std::byte const>> const &glyphDataList, uint16_t unitsPerEm)
{
    std::vector<Path> glyphs;

    for (size_t i = 0; i < glyphDataList.size(); i++) {
        let glyph = parseGlyph(glyphDataList, i, unitsPerEm);
        glyphs.push_back(glyph);
    }

    return glyphs;
}

static std::vector<gsl::span<std::byte const>> parseLOCA(gsl::span<std::byte const> bytes, gsl::span<std::byte const> glyfBytes, size_t numberOfGlyphs, bool longFormat)
{
    std::vector<gsl::span<std::byte const>> r;

    if (longFormat) {
        let longTable = make_span<big_uint32_buf_t>(bytes, 0, numberOfGlyphs + 1);

        for (size_t i = 0; i < numberOfGlyphs; i++) {
            let offset = static_cast<size_t>(longTable.at(i).value());
            let size = static_cast<size_t>(longTable.at(i + 1).value()) - offset;
            r.emplace_back(glyfBytes.subspan(offset, size));
        }
    } else {
        let shortTable = make_span<big_uint16_buf_t>(bytes, 0, numberOfGlyphs + 1);

        for (size_t i = 0; i < numberOfGlyphs; i++) {
            let offset = static_cast<size_t>(shortTable.at(i).value()) * 2;
            let size = static_cast<size_t>(shortTable.at(i + 1).value()) * 2 - offset;
            r.emplace_back(glyfBytes.subspan(offset, size));
        }
    }

    return r;
}

static void parseHMTX(std::vector<Path> &glyphs, gsl::span<std::byte const> horizontalMetricsData, HHEATable horizontalHeader, float xHeight, float HHeight, uint16_t unitsPerEm)
{
    let numberOfHMetrics = horizontalHeader.numberOfHMetrics.value();

    size_t offset = 0;
    let longHorizontalMetricTable = make_span<HMTXEntry>(horizontalMetricsData, offset, numberOfHMetrics);
    offset += numberOfHMetrics * sizeof(HMTXEntry);
    let leftSideBearings = make_span<FWord_buf_t>(horizontalMetricsData, offset, glyphs.size() - numberOfHMetrics);

    float advanceWidth = 0.0f;
    float leftSideBearing;
    for (size_t i = 0; i < glyphs.size(); i++) {
        if (i < numberOfHMetrics) {
            advanceWidth = longHorizontalMetricTable.at(i).advanceWidth.value(unitsPerEm);
            leftSideBearing = longHorizontalMetricTable.at(i).leftSideBearing.value(unitsPerEm);
        } else {
            //advanceWidth will be repeated.
            leftSideBearing = leftSideBearings.at(i - numberOfHMetrics).value(unitsPerEm);
        }

        auto &glyph = glyphs.at(i);
        glyph.advance = glm::vec2{advanceWidth, 0.0f};
        glyph.leftSideBearing = glm::vec2{leftSideBearing, 0.0f};
        glyph.rightSideBearing = glm::vec2{advanceWidth - (leftSideBearing + glyph.boundingBox.extent.width()), 0.0f};
        glyph.ascender = glm::vec2{0.0f, horizontalHeader.ascender.value(unitsPerEm)};
        glyph.descender = glm::vec2{0.0f, horizontalHeader.descender.value(unitsPerEm)};
        glyph.xHeight = glm::vec2{0.0f, xHeight};
        glyph.capHeight = glm::vec2{0.0f, HHeight};
    }
}

template<typename T=std::byte>
static gsl::span<T const> getSpanToTable(gsl::span<std::byte const> bytes, gsl::span<SFNTEntry const> const entries, uint32_t tag)
{
    for (let &entry: entries) {
        if (entry.tag.value() == tag) {
            return make_span<T>(bytes, entry.offset.value(), entry.length.value() / sizeof(T));
        }
    }
    return make_span<T>(bytes, 0, 0);
}

template<typename T = std::byte>
static T const getTable(gsl::span<std::byte const> bytes, gsl::span<SFNTEntry const> const entries, uint32_t tag)
{
    return getSpanToTable<T>(bytes, entries, tag).at(0);
}

Font parseTrueTypeFile(gsl::span<std::byte const> bytes)
{
    Font font;

    let &fontDirectory = at<SFNTHeader>(bytes, 0);
    if (!(fontDirectory.scalerType.value() == fourcc("true") || fontDirectory.scalerType.value() == 0x00010000)) {
        BOOST_THROW_EXCEPTION(ParseError("sfnt.scalerType is not 'true' or 0x00010000"));
    }

    let tableDirectory = make_span<SFNTEntry>(bytes, sizeof(SFNTHeader), fontDirectory.numTables.value());
    // required tables, tables are sorted alphabetically, but we need to read them in another order.
    // 'cmap'	character to glyph mapping
    // 'glyf'	glyph data
    // 'head'	font header
    // 'hhea'	horizontal header
    // 'hmtx'	horizontal metrics
    // 'loca'	index to location (not needed)
    // 'maxp'	maximum profile
    // 'name'	naming
    // 'post'	PostScript (not needed)

    let characterMapData = getSpanToTable(bytes, tableDirectory, fourcc("cmap"));
    font.characterMap = parseCMAP(characterMapData);

    let header = getTable<HEADTable>(bytes, tableDirectory, fourcc("head"));
    let locationLongFormat = header.indexToLocFormat.value() > 0;
    let unitsPerEm = header.unitsPerEm.value();

    let memoryRequirementTable = getTable<MAXPTable>(bytes, tableDirectory, fourcc("maxp"));
    let numGlyphs = memoryRequirementTable.numGlyphs.value();

    let locationTableData = getSpanToTable(bytes, tableDirectory, fourcc("loca"));
    let glyphTableData = getSpanToTable(bytes, tableDirectory, fourcc("glyf"));
    let glyphDataList = parseLOCA(locationTableData, glyphTableData, numGlyphs, locationLongFormat);

    font.glyphs = parseGLYF(glyphDataList, unitsPerEm);

    let xGlyphIndex = font.characterMap.at('x');
    let xGlyph = font.glyphs.at(xGlyphIndex);
    let xHeight = xGlyph.boundingBox.extent.height();

    let HGlyphIndex = font.characterMap.at('H');
    let HGlyph = font.glyphs.at(HGlyphIndex);
    let HHeight = HGlyph.boundingBox.extent.height();

    let horizontalHeader = getTable<HHEATable>(bytes, tableDirectory, fourcc("hhea"));

    let horizontalMetricsData = getSpanToTable(bytes, tableDirectory, fourcc("hmtx"));
    parseHMTX(font.glyphs, horizontalMetricsData, horizontalHeader, xHeight, HHeight, unitsPerEm);

    return font;
}

}
