
build:
	g++ -o readpass readPass.cpp

test: build
	python TestScript.py
