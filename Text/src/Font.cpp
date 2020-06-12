// Copyright 2019, 2020 Pokitec
// All rights reserved.

#include "TTauri/Text/Font.hpp"
#include "TTauri/Text/TrueTypeFont.hpp"
#include "TTauri/Foundation/ResourceView.hpp"

namespace tt {

[[nodiscard]] FontGlyphIDs Font::find_glyph(Grapheme g) const noexcept
{
    FontGlyphIDs r;

    // First try composed normalization
    for (ssize_t i = 0; i != ssize(g); ++i) {
        if (let glyph_id = find_glyph(g[i])) {
            r += glyph_id;
        } else {
            r.clear();
            break;
        }
    }

    if (!r) {
        // First try decomposed normalization
        for (let c: g.NFD()) {
            if (let glyph_id = find_glyph(c)) {
                r += glyph_id;
            } else {
                r.clear();
                break;
            }
        }
    }

    return r;
}

}

namespace tt {

template<>
std::unique_ptr<tt::Font> parseResource(URL const &location)
{
    if (location.extension() == "ttf") {
        auto view = ResourceView::loadView(location);

        try {
            return std::make_unique<tt::TrueTypeFont>(std::move(view));
        } catch (error &e) {
            e.set<url_tag>(location);
            throw;
        }

    } else {
        TTAURI_THROW(url_error("Unknown extension")
            .set<url_tag>(location)
        );
    }
}

}
