/*  Shiny Apple - shines up your Apple development environment.
    Copyright 2024 - Perry Kivolowitz
*/
#include <iostream>
#include <dirent.h>
#include <cassert>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

using std::string;
using std::cout;
using std::endl;
using std::cerr;

#define DEL_DOT_OHS     0x01
#define DEL_A_OUT       0x02
#define DEL_DOT_D       0x04
#define STOP_ON_ERROR   0x08
#define BE_QUIET        0x10

int main(int argc, char **argv) {
    void FindAndRemove(const string starting_folder, const uint32_t &);
    void HandleOptions(int, char **, uint32_t &, string &);
    string Initialize(const string);
    void PrintHelp();

    string starting_folder = ".";
    int retval = 0;
    uint32_t options = 0;

    try {
        HandleOptions(argc, argv, options, starting_folder);
        starting_folder = Initialize(starting_folder);
		FindAndRemove(starting_folder, options);
	} catch (string e) {
        if (e == "help") {
            PrintHelp();
        } else {
            cerr << e << endl;
            perror("");
            retval = 1;
        }
	}

	return retval;
}

string Initialize(const string starting_folder) {
    char buffer[PATH_MAX];

    if (chdir(starting_folder.c_str())) {
		throw string("Failed to open folder: ") + starting_folder;
    }
    if (!getcwd(buffer, PATH_MAX)) {
        throw "getcwd() improbably failed on: " + starting_folder;
    }
    return string(buffer);
}

void PrintHelp() {
    cout << "This program recursively removes Apple generated .dSym\n";
    cout << "folders containing debug symbols etc after compiling\n";
    cout << "a debug build. Also, optionally removes .o files left\n";
    cout << "over from the build process.\n";
    cout << "Usage:\n";
    cout << "-a       delete a.out files\n";
    cout << "-d       delete .d files\n";
    cout << "-o       delete .o files\n";
    cout << "-f path  set starting path - default is cwd\n";
	cout << "-h       print this help\n";
    cout << "-q       be quiet - supress printouts\n";
    cout << "-s       stop on errors otherwise keep going\n";
}

void HandleOptions(int argc, char **argv, uint32_t & options, string & starting_folder) {
    int c;

    while ((c = getopt(argc, argv, "qadhosf:")) != -1) {
        switch(c) {
            case 'h':
                throw string("help");
                break;

            case 'q':
                options |= BE_QUIET;
                break;

            case 's':
                options |= STOP_ON_ERROR;
                break;

			case 'd':
				options |= DEL_DOT_D;
				break;
			
            case 'a':
				options |= DEL_A_OUT;
				break;

			case 'o':
				options |= DEL_DOT_OHS;
                break;

            case 'f':
                starting_folder = string(optarg);
                break;
        }
    }
}

string Tail(std::string const & src, size_t const length) {
	if (length >= src.size()) {
		return "";
	}
	return src.substr(src.size() - length);
}

void FindAndRemove(const string starting_folder, const uint32_t & options) {
    DIR * sfolder = opendir(starting_folder.c_str());
    dirent * current_object = nullptr;
    
    if (!sfolder) {
        throw string("Failed to open folder: ") + starting_folder;
    }
    
    // This cannot fail as an opendir() succeeded on this path.
    (void) chdir(starting_folder.c_str());

    while ((current_object = readdir(sfolder)) != nullptr) {
		string fname = starting_folder + "/" + current_object->d_name;

        if (current_object->d_type == DT_REG) {
            bool do_unlink = false;
            if (options & DEL_DOT_OHS && Tail(string(current_object->d_name), 2) == ".o") {
                do_unlink = true;
			}
			if (options & DEL_DOT_OHS && Tail(string(current_object->d_name), 2) == ".d") {
				do_unlink = true;
			}
			if (options & DEL_DOT_OHS && string(current_object->d_name) == "a.out") {
				do_unlink = true;
			}
            if (do_unlink) {
    			if (!(options & BE_QUIET)) {
                    cout << "Deleting file:      " << fname << endl;
                }
	    		if (unlink(fname.c_str()) && options & STOP_ON_ERROR) {
                    throw "Deletion of " + fname + " failed";
                }
            }
		} else if (current_object->d_type == DT_DIR) {
            if (Tail(string(current_object->d_name), 5) == ".dSYM") {
				if (!(options & BE_QUIET)) {
					cout << "Deleting directory: " << fname << endl;
                }
				fname = "rm -rf " + fname;
				if (system(fname.c_str()) && options & STOP_ON_ERROR) {
					throw "Deletion of " + fname + " failed";
                }
                continue;
            } else if (string(current_object->d_name) == "." ||
				   string(current_object->d_name) == "..") {
                    continue;
            }
			string next_folder = starting_folder + "/" + current_object->d_name;
            FindAndRemove(next_folder, options);
            (void) chdir("..");
		}
	}
	(void) closedir(sfolder);
}
