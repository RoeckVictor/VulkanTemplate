import os
import re

folder_path = "VulkanTutorial/Source"
regex_to_match = r"\s*\/\/\s*\[COMMENT\]"

def delete_comments(file_path, comment_regex):
    lines_count = 0
    with open(file_path, "r") as f:
        lines = f.readlines()

    with open(file_path, "w") as f:
        for line in lines:
            if not re.match(regex_to_match, line):
                f.write(line)
            else:
                lines_count += 1
    
    print("Deleted " + str(lines_count) + " lines in file " + file_path)

def traverse_folder(folder_path, comment_regex):
    for root, dirs, files in os.walk(folder_path):
        for file_name in files:
            if file_name.endswith(".cpp") or file_name.endswith(".h"):
                file_path = os.path.join(root, file_name)
                delete_comments(file_path, comment_regex)

traverse_folder(folder_path, regex_to_match)