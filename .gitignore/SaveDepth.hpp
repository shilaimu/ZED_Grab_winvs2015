#pragma once
#ifndef __SAVE_DEPTH_HPP__
#define __SAVE_DEPTH_HPP__

#define NOMINMAX

#include <iomanip>
#include <signal.h>
#include <iostream>
#include <limits>
#include <thread>
//standard includes
#include <stdio.h>
#include <string.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sl_zed/Camera.hpp>

using namespace sl;
using namespace std;


const std::string helpString = "[d] Save Depth, [n] Change Depth format, [p] Save Point Cloud, [m] Change Point Cloud format, [q] Quit";
const std::string prefixPointCloud = "Cloud_"; // Default PointCloud output file prefix
const std::string prefixDepth = "Depth_"; // Default Depth image output file prefix
const std::string path = "./";
const std::string addr = ".";


std::string getPointCloudFormatName(sl::POINT_CLOUD_FORMAT f);
std::string getDepthFormatName(sl::DEPTH_FORMAT f);

void savePointCloud(sl::Camera& zed, std::string filename);
void saveDepth(sl::Camera& zed, std::string filename);
void saveSbSImage(sl::Camera& zed, std::string filename);
void saveImageSeq(Camera& zed);

void processKeyEvent(sl::Camera& zed, char &key);

void switchCameraSettings();
#endif
