/**
 * This file used a pattern language defined by imHex:
 * https://imhex.werwolv.net/docs/pattern_language/preprocessor.html
 * To use it, install ImHex and open your resource pack, then
 * parsing the binary pack by this script.
 */
char magic[9] @ 0x00;

enum Type : u8 {
    File = 0,
    Folder = 1
};

struct Entity {
    Type type;
    u8 reserved;
    u16 nameSize;
    u32 dataSize;
    char name[nameSize];
    if (type == Type::File) {
        u8 data[dataSize] [[hidden]];
    }
};

Entity e[while (std::mem::size() != $)] @$;
