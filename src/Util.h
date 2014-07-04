//
//  Util.h
//  sim
//
//  Created by Lucas Wolf on 3/30/14.
//
//

#ifndef __sim__Util__
#define __sim__Util__

#include <fstream>
#include <string>
#include <vector>
using namespace std;

//! Get current directory
extern string getCurrentDir();

//! SDKFile for the opencl program processing
class SDKFile {
public:
    //! Default constructor
    SDKFile(): source_(""){}
    
    //! Destructor
    ~SDKFile(){};
    
    //! Opens the CL program file
    bool open(const char* fileName);
    bool writeBinaryToFile(const char* fileName, const char* binary, size_t numBytes);
    bool readBinaryFromFile(const char* fileName);
    
    // Replaces Newline with spaces
    void replaceNewlineWithSpaces() {
        size_t pos = source_.find_first_of('\n', 0);
        while(pos != -1) {
            source_.replace(pos, 1, " ");
            pos = source_.find_first_of('\n', pos + 1);
        }
        pos = source_.find_first_of('\r', 0);
        while(pos != -1) {
            source_.replace(pos, 1, " ");
            pos = source_.find_first_of('\r', pos + 1);
        }
    }
    
    //! Returns a pointer to the string object with the source code
    const string& source() const {
        return source_;
    }
    
private:
    //! Disable copy constructor
    SDKFile(const SDKFile&);
    
    //! Disable operator=
    SDKFile& operator=(const SDKFile&);
    
    //!< source code of the CL program
    string source_;
};

#endif /* defined(__sim__Util__) */
