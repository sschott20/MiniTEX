# valgrind --track-origins=yes --leak-check=yes ./proj1 examples/t02 > me.t

import os
remove = ["output.txt", "combined_inputs.txt", "comments_removed.txt"]


for file in remove:
    if os.path.exists(file):
        os.remove(file)


os.system("make")
print("- Finish Make - ")
# examples/plaintext.txt examples/oneline.txt


# testfiles = ['examples/comment.txt']
# testfiles = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j']
# testfiles = [""]

# filelist = "/c/cs323/proj1/tests/t02"
filelist = "examples/t02.t"
# for file in testfiles:
#     filelist += " /c/cs323/proj1/tests/Files/" + file
    
# os.system("./proj1 " + filelist + " > me.t")
# os.system("/c/cs323/proj1/proj1 " + filelist+ " > right.t")

os.system("valgrind --track-origins=yes -q --leak-check=yes ./proj1 " + filelist + " > me.t")
os.system("/c/cs323/proj1/proj1 " + filelist+ " > right.t")
os.system("code --diff me.t right.t")

# numbers = ["{:02d}".format(n) for n in range(1, 50)]
# # os.system("echo /c/cs323/proj1/run_tests.pl " + " ".join(numbers))
# for number in numbers:
#     print("Test : " + number)
#     os.system("/c/cs323/proj1/tests/t" + number +" | cmp - /c/cs323/proj1/tests/t" + number +".t")


