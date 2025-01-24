#include "../include/led-matrix.h"
#include "wiringPi.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <exception>
#include <Magick++.h>
#include <magick/image.h>

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using rgb_matrix::FrameCanvas;
using ImageVector = std::vector<Magick::Image>;

Canvas *canvas;

static void LoadFace(const char *filename);
void drawFace(const Magick::Image &image, Canvas *canvas);
void changeFace (void);

int main(int argc, char *argv[]){
	wiringPiSetup();
	wiringPiISR(21, INT_EDGE_FALLING, &changeFace);

	RGBMatrix::Options defaults;
	defaults.hardware_mapping = "regular";
	defaults.rows = 32;
	defaults.cols = 64;
	defaults.chain_length = 2;
	defaults.parallel = 1;
	canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
	if (canvas == NULL){
		return 1;
	}
}
static void LoadFace(const char *filename){
	ImageVector face;

	try {
		readImages(&face, filename);
	} catch (std::exception &e){
		if(e.what()){
			fprintf(stderr, "%s\n", e.what());
		}
		return;
	}

	if (face.empty()){
		fprintf(stderr, "no image found");
	}

	drawFace(face[0], canvas);
}

void drawFace(const Magick::Image &image, Canvas *canvas){
	for(int i = 0; i < image.rows(); ++i){
		for(int j = 0; j < image.columns(); ++j){
			const Magick::Color &c = image.pixelColor(i, j);
			if(c.alphaQuantum() < 256){
				canvas->SetPixel (i, j,
					ScaleQuantumToChar(c.redQuantum()),
					ScaleQuantumToChar(c.greenQuantum()),
					ScaleQuantumToChar(c.blueQuantum()));
			}
		}
	}
}

void changeFace(void){
	LoadFace("Protogen-Neutral.png");
}
