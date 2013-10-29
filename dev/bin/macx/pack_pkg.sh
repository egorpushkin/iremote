#!/bin/bash
#
# This script creates Remote PC Suite installation package (.pkg).
#  

# Generate installer package.
/Developer/usr/bin/packagemaker \
	--title "Remote PC Suite" \
	--version 1.4 \
	--filter "\.DS_Store" \
	--root-volume-only \
	--domain system \
	--verbose \
	--no-relocate \
	--target 10.5 \
	--id com.scientific-soft.remotepcsuite.pkg \
	--root ./build \
	--out RemotePCSetup.pkg
