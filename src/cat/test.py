import subprocess

file = "tests/bytes.txt"
software = "s21_cat"

comps = 0

flags = list("benstv")
flags_len = len(flags) + 1
err_flags = []

str1 = subprocess.check_output([f'cat', f'{file}'])
str2 = subprocess.check_output([f'./{software}', f'{file}'])
if str1 == str2:
    comps+=1
else:
    err_flags.append("no arg")

for i in flags:
    str1 = subprocess.check_output([f'cat', f'-{i}', f'{file}'])
    str2 = subprocess.check_output([f'./{software}', f'-{i}', f'{file}'])
    if str1 == str2:
        comps+=1
    else:
        err_flags.append(i)

print(f"=============")
print(f"*    {comps}/{flags_len}    *")
print(f"=============")
if len(err_flags) != 0:
    print("errors")
    print(" ".join(err_flags))
