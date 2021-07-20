grep -hR -e '^struct' -e '^enum [^{]' kernel libc fs sh | sort | uniq -D
