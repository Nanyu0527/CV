#undef UNICODE
#include <windows.h>  
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>

using namespace std;

int readFilenames(vector<string>& filenames, const string& directory) {
	
	WIN32_FIND_DATA data;
	HANDLE dir = FindFirstFile((directory + "/*").c_str(), &data);
	if (dir == INVALID_HANDLE_VALUE)
		return 0;
	do {
		const string file_name = data.cFileName;
		const string full_file_name = directory + "/" + file_name;
		const bool is_directory = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		if (file_name[0] == '.')
			continue;
		if (is_directory)
			continue;
		filenames.push_back(full_file_name);
	} while (FindNextFile(dir, &data));

	FindClose(dir);
	std::sort(filenames.begin(), filenames.end());  //sort the name of files
	return(filenames.size());
}  // GetFilesInDirectory


int main() {
	float image_sf = 0.5f;    // image scaling factor
	int delay = 250;          // miliseconds
	int board_w = 0;
	int board_h = 0;
	cout << "input the width number of board corners:" << endl;
	cin >> board_w;
	cout << "input the height number of board corners:" << endl;
	cin >> board_h;
	int n_boards;
	cout << "input the number of board:" << endl;
	cin >> n_boards;// how many boards max to read
	cout << "input the milisecond delay" << endl;
	cin >> delay;
	cout << "input the image_scaling_factor:" << endl;
	cin >> image_sf;
	string folder;
	cout << "input the name of folder:" << endl;
	cin >> folder;
	string chessB;
	cout << "input path to chessboard" << endl;
	cin >> chessB;

	int board_n = board_w * board_h;  // number of corners
	cv::Size board_sz = cv::Size(board_w, board_h);  // width and height of the board
	cout << "Reading in directory " << folder << endl;
	vector<string> filenames;
	int num_files = readFilenames(filenames, folder);
	cout << "   ... Done. Number of files = " << num_files << "\n" << endl;

	vector<vector<cv::Point2f> > image_points;
	vector<vector<cv::Point3f> > object_points;

	// collecting all corners on the boards that are found
	cv::Size image_size;
	int board_count = 0;
	for (size_t i = 0; (i < filenames.size()) && (board_count < n_boards); ++i) {
		cv::Mat image, image0 = cv::imread(folder + filenames[i]);
		board_count += 1;
		if (!image0.data) {  // protect against no file
			cerr << folder + filenames[i] << ", file #" << i << ", is not an image" << endl;
			continue;
		}
		image_size = image0.size();
		cv::resize(image0, image, cv::Size(), image_sf, image_sf, cv::INTER_LINEAR);

		vector<cv::Point2f> corners;
		bool found = cv::findChessboardCorners(image, board_sz, corners);

		// Draw 
		drawChessboardCorners(image, board_sz, corners, found);  // will draw only if found
		// If found , add it to our data
		if (found) {
			image ^= cv::Scalar::all(255);
			cv::Mat mcorners(corners);
			// do not copy the data
			mcorners *= (1.0 / image_sf);
			// scale the corner coordinates
			image_points.push_back(corners);
			object_points.push_back(vector<cv::Point3f>());
			vector<cv::Point3f>& opts = object_points.back();

			opts.resize(board_n);
			for (int j = 0; j < board_n; j++) {
				opts[j] = cv::Point3f(static_cast<float>(j / board_w),
					static_cast<float>(j % board_w), 0.0f);
			}
		}
		cv::imshow("Calibration", image);

		if ((cv::waitKey(delay) & 255) == 27) {
			return -1;
		}
	}

	cv::destroyWindow("Calibration");
	cout << "\n\n*** CALIBRATING THE CAMERA...\n" << endl;

	cv::Mat intrinsic_matrix, distortion_coeffs;
	double err = cv::calibrateCamera(
		object_points, image_points, image_size, intrinsic_matrix,
		distortion_coeffs, cv::noArray(), cv::noArray(),
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT);

	cout << " *** DONE!\n\nReprojection error is " << err
		<< "\nStoring Intrinsics.xml and Distortions.xml files\n\n";
	cv::FileStorage fs("intrinsics.xml", cv::FileStorage::WRITE);
	fs << "image_width" << image_size.width << "image_height" << image_size.height
		<< "camera_matrix" << intrinsic_matrix << "distortion_coefficients"
		<< distortion_coeffs;
	fs.release();

	fs.open("intrinsics.xml", cv::FileStorage::READ);
	cout << "\nimage width: " << static_cast<int>(fs["image_width"]);
	cout << "\nimage height: " << static_cast<int>(fs["image_height"]);
	cv::Mat intrinsic_matrix_loaded, distortion_coeffs_loaded;
	fs["camera_matrix"] >> intrinsic_matrix_loaded;
	fs["distortion_coefficients"] >> distortion_coeffs_loaded;
	cout << "\nintrinsic matrix:" << intrinsic_matrix_loaded;
	cout << "\ndistortion coefficients: " << distortion_coeffs_loaded << "\n" << endl;

	// Build the undistort map
	cv::Mat map1, map2;
	cv::initUndistortRectifyMap(intrinsic_matrix_loaded, distortion_coeffs_loaded,
		cv::Mat(), intrinsic_matrix_loaded, image_size,
		CV_16SC2, map1, map2);

	//display
	board_count = 0;  // resent max boards to read
	for (size_t i = 0; (i < filenames.size()) && (board_count < n_boards); ++i) {
		cv::Mat image, image0 = cv::imread(folder + filenames[i]);
		++board_count;
		if (!image0.data) {  // protect against no file
			cerr << folder + filenames[i] << ", file #" << i << ", is not an image" << endl;
			continue;
		}
		cv::remap(image0, image, map1, map2, cv::INTER_LINEAR,
			cv::BORDER_CONSTANT, cv::Scalar());
		cv::imshow("Original", image0);
		cv::imshow("Undistorted", image);
		if ((cv::waitKey(0) & 255) == 27) {
			break;
		}
	}
	cv::destroyWindow("Original");
	cv::destroyWindow("Undistorted");

	//birdview
	cv::FileStorage fs2("intrinsics.xml", cv::FileStorage::READ);
	cv::Mat intrinsic, distortion;
	fs2["camera_matrix"] >> intrinsic;
	fs2["distortion_coefficients"] >> distortion;
	if (!fs2.isOpened() || intrinsic.empty() || distortion.empty()) {
		cout << "Error: Couldn't load intrinsic parameters" << endl;
		return -1;
	}
	fs2.release();

	cv::Mat gray_image, image, image0 = cv::imread(chessB, 1);
	if (image0.empty()) {
		cout << "Error: Couldn't load image " << chessB << endl;
		return -1;
	}
	cv::undistort(image0, image, intrinsic, distortion, intrinsic);
	cv::cvtColor(image, gray_image, cv::COLOR_BGRA2GRAY);

	vector<cv::Point2f> corners;
	bool found = cv::findChessboardCorners( // True if found
		image,                              // Input image
		board_sz,                           // Pattern size
		corners,                            // Results
		cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS);
	if (!found) {
		cout << "Couldn't acquire checkerboard on " << chessB << ", only found "
			<< corners.size() << " of " << board_n << " corners\n";
		return -1;
	}

	// Get Subpixel accuracy on those corners
	cv::cornerSubPix(
		gray_image,       // Input image
		corners,          // Initial guesses, also output
		cv::Size(11, 11), // Search window size
		cv::Size(-1, -1), // Zero zone (in this case, don't use)
		cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 30,
			0.1));

	cv::Point2f objPts[4], imgPts[4];
	objPts[0].x = 0;
	objPts[0].y = 0;
	objPts[1].x = board_w - 1;
	objPts[1].y = 0;
	objPts[2].x = 0;
	objPts[2].y = board_h - 1;
	objPts[3].x = board_w - 1;
	objPts[3].y = board_h - 1;
	imgPts[0] = corners[0];
	imgPts[1] = corners[board_w - 1];
	imgPts[2] = corners[(board_h - 1) * board_w];
	imgPts[3] = corners[(board_h - 1) * board_w + board_w - 1];

	cv::circle(image, imgPts[0], 9, cv::Scalar(255, 0, 0), 3);
	cv::circle(image, imgPts[1], 9, cv::Scalar(0, 255, 0), 3);
	cv::circle(image, imgPts[2], 9, cv::Scalar(0, 0, 255), 3);
	cv::circle(image, imgPts[3], 9, cv::Scalar(0, 255, 255), 3);

	cv::drawChessboardCorners(image, board_sz, corners, found);
	cv::imshow("Checkers", image);

	cv::Mat H = cv::getPerspectiveTransform(objPts, imgPts);

	cout << "\nPress 'd' for lower birdseye view, and 'u' for higher (it adjusts the apparent 'Z' height), Esc to exit" << endl;
	double Z = 15;
	cv::Mat birds_image;
	for (;;) {
		// escape key stops
		H.at<double>(2, 2) = Z;

		cv::warpPerspective(image,			// Source image
			birds_image, 	// Output image
			H,              // Transformation matrix
			image.size(),   // Size for output image
			cv::WARP_INVERSE_MAP | cv::INTER_LINEAR,
			cv::BORDER_CONSTANT, cv::Scalar::all(0) // Fill border with black
		);
		cv::imshow("Birds_Eye", birds_image);
		int key = cv::waitKey() & 255;
		if (key == 'u')
			Z += 0.5;
		if (key == 'd')
			Z -= 0.5;
		if (key == 27)
			break;
	}

	vector<cv::Point2f> image_points2;
	vector<cv::Point3f> object_points2;
	for (int i = 0; i < 4; ++i) {
		image_points2.push_back(imgPts[i]);
		object_points2.push_back(cv::Point3f(objPts[i].x, objPts[i].y, 0));
	}
	cv::Mat rvec, tvec, rmat;
	cv::solvePnP(object_points2, 	// 3-d points in object coordinate
		image_points2,  	// 2-d points in image coordinates
		intrinsic,     	// Our camera matrix
		cv::Mat(),     	// Since we corrected distortion in the
		// beginning,now we have zero distortion
		// coefficients
		rvec, 			// Output rotation *vector*.
		tvec  			// Output translation vector.
	);
	cv::Rodrigues(rvec, rmat);

	cout << "rotation matrix: " << rmat << endl;
	cout << "translation vector: " << tvec << endl;
	cout << "homography matrix: " << H << endl;
	cout << "inverted homography matrix: " << H.inv() << endl;

	return 0;
}