// Not building by default since a zig environment is not common or something
// I want to require.
//
// zig build-obj modules/zigmod.zig -target x86_64-freestanding -Isysroot/usr/include -femit-bin=sysroot/bin/zigmod.ko
const ng = @cImport({
    @cInclude("basic.h");
    @cInclude("stdio.h");
    @cInclude("ng/mod.h");
});

export fn init(mod: [*c]ng.mod) c_int {
    _ = ng.printf("Hello World from a zig kernel module\n");
    return ng.MODINIT_SUCCESS;
}

export const modinfo: ng.modinfo = ng.modinfo{
    .init = init,
    .fini = null,
    .name = "zigmod",
};
