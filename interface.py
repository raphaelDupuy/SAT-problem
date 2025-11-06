import subprocess
import sys

if __name__ == '__main__':

    dir = "./bin/"

    args = sys.argv[1:]
    match (args[0]):
        case "chrono":
            dir += "chrono"
            arguments = (dir, args[1], args[2])
        case "compare":
            dir += "compare"
            arguments = (dir, args[1], args[2])
        case "resolve":
            dir += "resolve"
            if (len(args) == 2):
                arguments = (dir, args[1])
            elif (len(args) == 3):
                arguments = (dir, args[1], args[2])
        case _:
            pass

    result = subprocess.run(arguments).stdout
