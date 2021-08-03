trait Socket {
    fn send(&[u8]) -> core::result::Result<(), ()>
}

struct LoopbackSocket {
    data: Vec<u8>,
}

impl Socket for LoopbackSocket {

}
