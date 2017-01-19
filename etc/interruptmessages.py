
import csv

def main():
    with open("etc/interrupt-descs.txt") as f:
        read = csv.reader(f)
        for row in read:
            g = ""
            if row[2] == "Yes":
                g = "push 0"
            print("\"{}\",".format(row[1]))

if __name__ == "__main__":
    main()
