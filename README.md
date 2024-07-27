# Simple AV
A simple YARA rule based file scanner built for my Clemson CSOC summer semester project

## How to use (Don't)

Place Yara rules in the top level of the rules directory, all rules will be compiled the first time the program is ran.

Single file scan:  (TODO)  
`$ ./simple_av -f <path_to_file>`

Monitor mode: (TODO)  
`$ ./simple_av -p <path_of_dir>`

## Resources
[libyara](https://github.com/VirusTotal/yara)  
[Makefile (Courtesy of smlbfstr)](https://github.com/smlbfstr/Makefile)