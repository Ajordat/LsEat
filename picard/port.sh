#!/bin/bash

if ! lsof -i:$1
then
	echo $1 is free
else 
	echo $1 is occupied
fi


