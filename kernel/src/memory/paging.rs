
struct Page {}

struct PML4 {
    entries: [PML4Entry; 512],
}
struct PML4Entry {
    data: u64,
}

struct PDPT {
    entries: [PDPTEntry; 512],
}
struct PDPTEntry {
    data: u64,
}

struct PD {
    entries: [PDEntry; 512],
}
struct PDEntry {
    data: u64,
}

struct PT {
    entries: [PTEntry; 512],
}
struct PTEntry {
    data: u64,
}

