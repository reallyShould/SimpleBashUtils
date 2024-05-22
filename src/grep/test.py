import subprocess

file = "pattern.txt"
file2 = "pattern2.txt"
software = "s21_grep"
or_software = "grep"

patterns = list('qwertyuioplkjhgfdaszxcvbnm1234567890') + ['he', "re", " ", "reallys"]
flagsSimple = list("eivclnhs")
flagsPair = ['vi', 'ni', 'lv', 'cv', 'nv', 'li', 'vs', 'is']
flags = flagsSimple + flagsPair

def run_test(software, flag, pattern, files):
    try:
        result = subprocess.check_output([software, f'-{flag}', pattern, *files])
    except subprocess.CalledProcessError:
        result = b''
    return result.decode('utf-8').strip()

def compare_results(flag, pattern, files):
    str1 = run_test(or_software, flag, f"{pattern}", files)
    str2 = run_test(f'./{software}', flag, f"{pattern}", files)
    if str1 != str2:
        print(f"Difference found with flag -{flag} and pattern '{pattern}':")
        print(f"===Expected===\n{str1}")
        print(f"=====Got======\n{str2}")
    return str1 == str2

def test_patterns_with_flags(flags, patterns, files):
    comps = 0
    err_flags = []
    total_tests = len(flags) * len(patterns)

    for flag in flags:
        for pattern in patterns:
            if compare_results(flag, pattern, files):
                comps += 1
            else:
                err_flags.append(flag)

    return comps, total_tests, err_flags

# Test with single file
comps, flags_len, err_flags = test_patterns_with_flags(flags, patterns, [file])
print("\nWith pair flags")
print(f"==================")
print(f"*Simple file test*")
print(f"*     {comps}/{flags_len}    *")
print(f"==================")
if err_flags:
    print("Errors:")
    print(" ".join(set(err_flags)))

# Test with multiple files
comps, flags_len, err_flags = test_patterns_with_flags(flags, patterns, [file, file2])
print(f"==================")
print(f"*Multi file test *")
print(f"*     {comps}/{flags_len}    *")
print(f"==================")
if err_flags:
    print("Errors:")
    print(" ".join(set(err_flags)))
