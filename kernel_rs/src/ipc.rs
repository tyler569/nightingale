use intrusive_collections::intrusive_adapter;
use intrusive_collections::{LinkedList, LinkedListLink};

struct MailboxMessage {
    link: LinkedListLink,
    data: [u8],
}

intrusive_adapter!(MailLink = Box<MailboxMessage>: MailboxMessage { link: LinkedListLink });

struct Mailbox {
    queue: LinkedList<MailLink>,
}
