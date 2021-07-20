# destroy vs close

## Who calls close?

`file->ops->close(ofd)` should be called when you have an `open_file`
that is going away. `close` will perform any filetype-specific closing
behavior required, such as changing reader/writer counts or waking
readers.

`file->ops->destroy(file)` should be called when an operation is
triggered that could cause the file to cease to exist, such as removing
it from the directory tree or closing a socket. `destroy` must be called
from all implementations of `close`, because removing the last reference
to a file may destroy it. The function is seperate because there are
circumstances where a file may be potentially destoryed that do not
coincide with having a valid open\_file, such as removing it from a
directory.  
`destroy` will do nothing if there is actually some open reference, 

