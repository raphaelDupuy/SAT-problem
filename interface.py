import subprocess
import sys

if __name__ == '__main__':

    dir = "./bin/"

    args = sys.argv[1:]
    match (args[0]):
        case "chrono":
            dir += "chrono"
            # + Pb / DB
        case "compare":
            dir += "compare"
            # + ens des algo à comparer
        case "resolve":
            dir += "resolve"
        case _:
            pass

    arguments = (dir, args[1], args[2])
    result = subprocess.run(arguments).stdout
