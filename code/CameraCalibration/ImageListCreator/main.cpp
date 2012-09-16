//
//  main.cpp
//  ImageListCreator
//
//  Created by Christoph Kapffer on 16.09.12.
//  Copyright (c) 2012 Christoph Kapffer. All rights reserved.
//
//  based on: https://github.com/Itseez/opencv/blob/master/samples/cpp/imagelist_creator.cpp

/*this creates a yaml or xml list of files from the command line args
 */

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <iostream>
#include <dirent.h> // doesn't work for windows

using namespace std;
using namespace cv;

static void help(char** av)
{
    cout << "\nThis creates a yaml or xml list of files from the command line args\n"
    "usage:\n./" << av[0] << " imagelist.yaml ./imageFolder\n"
    << "Try using different extensions.(e.g. yaml yml xml xml.gz etc...)\n"
    << "This will serialize this list of images or whatever with opencv's FileStorage framework" << endl;
}

bool isImage(const string& fileName)
{
    string extension = fileName.substr(fileName.find_last_of(".") + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension == "jpg" || extension == "png" || extension == "bmp"; // TODO: add more extensions
}

int main(int ac, char** av)
{
    if (ac < 3)
    {
        help(av);
        return 1;
    }
    
    string outputname = av[1];
    string inputDirName = av[2];
    
    Mat m = imread(outputname); //check if the output is an image - prevent overwrites!
    if(!m.empty()){
        cerr << "fail! Please specify an output file, don't want to overwrite you images!" << endl;
        help(av);
        return 1;
    }
    
    DIR *dir;
    struct dirent *ent;
    dir = opendir(inputDirName.c_str());
    if (dir != NULL) {
        
        FileStorage fs(outputname, FileStorage::WRITE);
        fs << "images" << "[";
        while ((ent = readdir (dir)) != NULL) {
            string fileName = string(ent->d_name);
            if (isImage(fileName)) {
                fs << (inputDirName + "/" + fileName);
            }
        }
        fs << "]";
        return 0;
    }
    
    cerr << "Could not open directory " << av[2] << endl;
    return 1;
}
