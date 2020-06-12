// Copyright 2020 Pokitec
// All rights reserved.

#pragma once

#include "TTauri/Text/FontID.hpp"
#include "TTauri/Text/FontGlyphIDs.hpp"
#include "TTauri/Text/FontBook.hpp"

namespace tt {

inline FontID ElusiveIcons_font_id = FontID{};

enum class ElusiveIcon : char32_t {
    AddressBook = 0xf102,
    AddressBookAlt = 0xf101,
    Adjust = 0xf104,
    AdjustAlt = 0xf103,
    Adult = 0xf105,
    AlignCenter = 0xf106,
    AlignJustify = 0xf107,
    AlignLeft = 0xf108,
    AlignRight = 0xf109,
    ArrowDown = 0xf10a,
    ArrowLeft = 0xf10b,
    ArrowRight = 0xf10c,
    ArrowUp = 0xf10d,
    Asl = 0xf10e,
    Asterisk = 0xf10f,
    Backward = 0xf110,
    BanCircle = 0xf111,
    Barcode = 0xf112,
    Behance = 0xf113,
    Bell = 0xf114,
    Blind = 0xf115,
    Blogger = 0xf116,
    Bold = 0xf117,
    Book = 0xf118,
    Bookmark = 0xf11a,
    BookmarkEmpty = 0xf119,
    Braille = 0xf11b,
    Briefcase = 0xf11c,
    Broom = 0xf11d,
    Brush = 0xf11e,
    Bulb = 0xf11f,
    Bullhorn = 0xf120,
    Calendar = 0xf122,
    CalendarSign = 0xf121,
    Camera = 0xf123,
    Car = 0xf124,
    CaretDown = 0xf125,
    CaretLeft = 0xf126,
    CaretRight = 0xf127,
    CaretUp = 0xf128,
    Cc = 0xf129,
    Certificate = 0xf12a,
    Check = 0xf12c,
    CheckEmpty = 0xf12b,
    ChevronDown = 0xf12d,
    ChevronLeft = 0xf12e,
    ChevronRight = 0xf12f,
    ChevronUp = 0xf130,
    Child = 0xf131,
    CircleArrowDown = 0xf132,
    CircleArrowLeft = 0xf133,
    CircleArrowRight = 0xf134,
    CircleArrowUp = 0xf135,
    Cloud = 0xf137,
    CloudAlt = 0xf136,
    Cog = 0xf139,
    CogAlt = 0xf138,
    Cogs = 0xf13a,
    Comment = 0xf13c,
    CommentAlt = 0xf13b,
    Compass = 0xf13e,
    CompassAlt = 0xf13d,
    CreditCard = 0xf13f,
    Css = 0xf140,
    Dashboard = 0xf141,
    Delicious = 0xf142,
    Deviantart = 0xf143,
    Digg = 0xf144,
    Download = 0xf146,
    DownloadAlt = 0xf145,
    Dribbble = 0xf147,
    Edit = 0xf148,
    Eject = 0xf149,
    Envelope = 0xf14b,
    EnvelopeAlt = 0xf14a,
    Error = 0xf14d,
    ErrorAlt = 0xf14c,
    Eur = 0xf14e,
    ExclamationSign = 0xf14f,
    EyeClose = 0xf150,
    EyeOpen = 0xf151,
    Facebook = 0xf152,
    FacetimeVideo = 0xf153,
    FastBackward = 0xf154,
    FastForward = 0xf155,
    Female = 0xf156,
    File = 0xf15c,
    FileAlt = 0xf157,
    FileEdit = 0xf159,
    FileEditAlt = 0xf158,
    FileNew = 0xf15b,
    FileNewAlt = 0xf15a,
    Film = 0xf15d,
    Filter = 0xf15e,
    Fire = 0xf15f,
    Flag = 0xf161,
    FlagAlt = 0xf160,
    Flickr = 0xf162,
    Folder = 0xf166,
    FolderClose = 0xf163,
    FolderOpen = 0xf164,
    FolderSign = 0xf165,
    Font = 0xf167,
    Fontsize = 0xf168,
    Fork = 0xf169,
    Forward = 0xf16b,
    ForwardAlt = 0xf16a,
    Foursquare = 0xf16c,
    Friendfeed = 0xf16e,
    FriendfeedRect = 0xf16d,
    Fullscreen = 0xf16f,
    Gbp = 0xf170,
    Gift = 0xf171,
    Github = 0xf173,
    GithubText = 0xf172,
    Glass = 0xf174,
    Glasses = 0xf175,
    Globe = 0xf177,
    GlobeAlt = 0xf176,
    Googleplus = 0xf178,
    Graph = 0xf17a,
    GraphAlt = 0xf179,
    Group = 0xf17c,
    GroupAlt = 0xf17b,
    Guidedog = 0xf17d,
    HandDown = 0xf17e,
    HandLeft = 0xf17f,
    HandRight = 0xf180,
    HandUp = 0xf181,
    Hdd = 0xf182,
    Headphones = 0xf183,
    HearingImpaired = 0xf184,
    Heart = 0xf187,
    HeartAlt = 0xf185,
    HeartEmpty = 0xf186,
    Home = 0xf189,
    HomeAlt = 0xf188,
    Hourglass = 0xf18a,
    Idea = 0xf18c,
    IdeaAlt = 0xf18b,
    Inbox = 0xf18f,
    InboxAlt = 0xf18d,
    InboxBox = 0xf18e,
    IndentLeft = 0xf190,
    IndentRight = 0xf191,
    InfoCircle = 0xf192,
    Instagram = 0xf193,
    IphoneHome = 0xf194,
    Italic = 0xf195,
    Key = 0xf196,
    Laptop = 0xf198,
    LaptopAlt = 0xf197,
    Lastfm = 0xf199,
    Leaf = 0xf19a,
    Lines = 0xf19b,
    Link = 0xf19c,
    Linkedin = 0xf19d,
    List = 0xf19f,
    ListAlt = 0xf19e,
    Livejournal = 0xf1a0,
    Lock = 0xf1a2,
    LockAlt = 0xf1a1,
    Magic = 0xf1a3,
    Magnet = 0xf1a4,
    Male = 0xf1a5,
    MapMarker = 0xf1a7,
    MapMarkerAlt = 0xf1a6,
    Mic = 0xf1a9,
    MicAlt = 0xf1a8,
    Minus = 0xf1ab,
    MinusSign = 0xf1aa,
    Move = 0xf1ac,
    Music = 0xf1ad,
    Myspace = 0xf1ae,
    Network = 0xf1af,
    Off = 0xf1b0,
    Ok = 0xf1b3,
    OkCircle = 0xf1b1,
    OkSign = 0xf1b2,
    Opensource = 0xf1b4,
    PaperClip = 0xf1b6,
    PaperClipAlt = 0xf1b5,
    Path = 0xf1b7,
    Pause = 0xf1b9,
    PauseAlt = 0xf1b8,
    Pencil = 0xf1bb,
    PencilAlt = 0xf1ba,
    Person = 0xf1bc,
    Phone = 0xf1be,
    PhoneAlt = 0xf1bd,
    Photo = 0xf1c0,
    PhotoAlt = 0xf1bf,
    Picasa = 0xf1c1,
    Picture = 0xf1c2,
    Pinterest = 0xf1c3,
    Plane = 0xf1c4,
    Play = 0xf1c7,
    PlayAlt = 0xf1c5,
    PlayCircle = 0xf1c6,
    Plurk = 0xf1c9,
    PlurkAlt = 0xf1c8,
    Plus = 0xf1cb,
    PlusSign = 0xf1ca,
    Podcast = 0xf1cc,
    Print = 0xf1cd,
    Puzzle = 0xf1ce,
    Qrcode = 0xf1cf,
    Question = 0xf1d1,
    QuestionSign = 0xf1d0,
    QuoteAlt = 0xf1d2,
    QuoteRight = 0xf1d4,
    QuoteRightAlt = 0xf1d3,
    Quotes = 0xf1d5,
    Random = 0xf1d6,
    Record = 0xf1d7,
    Reddit = 0xf1d8,
    Redux = 0xf1d9,
    Refresh = 0xf1da,
    Remove = 0xf1dd,
    RemoveCircle = 0xf1db,
    RemoveSign = 0xf1dc,
    Repeat = 0xf1df,
    RepeatAlt = 0xf1de,
    ResizeFull = 0xf1e0,
    ResizeHorizontal = 0xf1e1,
    ResizeSmall = 0xf1e2,
    ResizeVertical = 0xf1e3,
    ReturnKey = 0xf1e4,
    Retweet = 0xf1e5,
    ReverseAlt = 0xf1e6,
    Road = 0xf1e7,
    Rss = 0xf1e8,
    Scissors = 0xf1e9,
    Screen = 0xf1eb,
    ScreenAlt = 0xf1ea,
    Screenshot = 0xf1ec,
    Search = 0xf1ee,
    SearchAlt = 0xf1ed,
    Share = 0xf1f0,
    ShareAlt = 0xf1ef,
    ShoppingCart = 0xf1f2,
    ShoppingCartSign = 0xf1f1,
    Signal = 0xf1f3,
    Skype = 0xf1f4,
    Slideshare = 0xf1f5,
    Smiley = 0xf1f7,
    SmileyAlt = 0xf1f6,
    Soundcloud = 0xf1f8,
    Speaker = 0xf1f9,
    Spotify = 0xf1fa,
    Stackoverflow = 0xf1fb,
    Star = 0xf1fe,
    StarAlt = 0xf1fc,
    StarEmpty = 0xf1fd,
    StepBackward = 0xf1ff,
    StepForward = 0xf200,
    Stop = 0xf202,
    StopAlt = 0xf201,
    Stumbleupon = 0xf203,
    Tag = 0xf204,
    Tags = 0xf205,
    Tasks = 0xf206,
    TextHeight = 0xf207,
    TextWidth = 0xf208,
    Th = 0xf20b,
    ThLarge = 0xf209,
    ThList = 0xf20a,
    ThumbsDown = 0xf20c,
    ThumbsUp = 0xf20d,
    Time = 0xf20f,
    TimeAlt = 0xf20e,
    Tint = 0xf210,
    Torso = 0xf211,
    Trash = 0xf213,
    TrashAlt = 0xf212,
    Tumblr = 0xf214,
    Twitter = 0xf215,
    UniversalAccess = 0xf216,
    Unlock = 0xf218,
    UnlockAlt = 0xf217,
    Upload = 0xf219,
    Usd = 0xf21a,
    User = 0xf21b,
    Viadeo = 0xf21c,
    Video = 0xf21f,
    VideoAlt = 0xf21d,
    VideoChat = 0xf21e,
    ViewMode = 0xf220,
    Vimeo = 0xf221,
    Vkontakte = 0xf222,
    VolumeDown = 0xf223,
    VolumeOff = 0xf224,
    VolumeUp = 0xf225,
    W3c = 0xf226,
    WarningSign = 0xf227,
    Website = 0xf229,
    WebsiteAlt = 0xf228,
    Wheelchair = 0xf22a,
    Wordpress = 0xf22b,
    Wrench = 0xf22d,
    WrenchAlt = 0xf22c,
    Youtube = 0xf22e,
    ZoomIn = 0xf22f,
    ZoomOut = 0xf230,
};

inline FontGlyphIDs to_FontGlyphIDs(ElusiveIcon rhs) noexcept {
    ttauri_assume(ElusiveIcons_font_id);
    ttauri_assume(fontBook);

    return fontBook->find_glyph(ElusiveIcons_font_id, Grapheme{static_cast<char32_t>(rhs)});
}



}
