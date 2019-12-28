
alias make='make -j16'
alias make32='make ARCH=I686'
alias make64='make ARCH=X86_64'

alias r32='./run.rb -32'
alias r64='./run.rb -64'

function mk() {
    make32 &
    make64 &
}

