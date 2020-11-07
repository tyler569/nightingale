objdump -t ngk.elf | grep '\.text' | sort | awk -f render_symbols.awk
