#!/bin/bash

elfDir=$(pwd)
testsDir="${elfDir}/tests"
curTestDir="${testsDir}/test"
Modifier="re_"
createError="Failed to complete creation"
dumpError="Failed to complete extraction"
extractNoneError="No such file is present in"
listError="Failed to complete list"

fileMode="644"
dirMode="777"
t8AdditionalPath="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"

clean () {
	#echo "cleanup starting"
	#pwd
	#$tarPath="${1}/*.tar"	
	#$dumpPath="${1}/*Dump"	

	hFiles="hiddenFiles"
	rm -f *.tar
	rm -rf *Dump 
	rm -f tarStructure 
	rm -f *.log
	rm -f genOutput
	if [ -d "$hFiles" ]; then
		cd "$hFiles"
		mv * ..
		cd ..
		rmdir "$hFiles"
	fi

	#pwd
	#echo "cleanup ending"
}

testCD () {
	local curTestDir="$curTestDir$1"
	if [ $1 -eq 8 ];then
		curTestDir="$curTestDir$t8AdditionalPath"
	fi
	#echo $curTestDir
	cd "$curTestDir"

	tarName=$3
	dumpName="testDump"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	#gen files needed for test case 4
	if [ $1 -eq 2 ] || [ $1 -eq 4 ]; then
		if [ -f "files.zip" ]; then
			unzip -q "files.zip"
			rm files.zip
		fi
	fi	

	cd "$elfDir"

	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	#errorFileSize=$(stat -c %s $errorPath)

	#multiple times tarring for testcase 5
	if [ $1 -eq 5 ]; then
		#echo "Creating tar file"
		tarName="$Modifier$tarName"
		#echo $tarName
		timeout 5m ./myTar -c "$curTestDir" $tarName 1>>"$creationErrorPath" 2>&1


		#echo "Creating tar file"
		tarName="$Modifier$tarName"
		#echo $tarName
		timeout 5m ./myTar -c "$curTestDir" $tarName 1>>"$creationErrorPath" 2>&1

	
		tarPath="${curTestDir}/"
		#echo $tarPath
		tarPath="$tarPath$tarName"
		#echo $tarPath
		dumpPath="${curTestDir}/"
		#echo $dumpPath
		dumpPath="$dumpPath$Modifier$Modifier$dumpName"
		#echo $dumpPath

	fi

	#echo "Hiding original files"
	mkdir "$hiddenPath"
	j=1
	while [ "$j" -le "$numFiles" ]; do
		#echo $j
		origFile="${curTestDir}/"
		origFile="$origFile$j"
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		j=$(($j + 1))
	done

	#extra files to be hidden in case of testcase 5
	if [ $1 -eq 5 ]; then
		origFile="${curTestDir}/"
		#echo $origFile
		origFile="$origFile$3"
		#echo $origFile
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1

		origFile="${curTestDir}/"
		origFile="$origFile$Modifier"
		origFile="$origFile$3"
		#echo $origFile
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1

		numFiles=$(expr $numFiles + 2)
		#echo $numFiles
	fi

	#echo "Extracting files from tar file"
	timeout 5m ./myTar -d "$tarPath" 1>"$errorPath" 2>&1

	#9th test case, special handling
	if [ $1 -eq 9 ]; then
		out="000"
		if [ -f "$tarPath" ];then
			out=$(stat -c %a "$tarPath" 2>"/dev/null")
		fi
		#echo $out
		if [ "$out" -ne "$fileMode" ]; then
			echo "Test$1: FAILED"
			return -1
		fi
	
		out="000"
		if [ -d "$dumpPath" ];then
			out=$(stat -c %a "$dumpPath" 2>"/dev/null")
		fi
		out=$(stat -c %a "$dumpPath" 2>"/dev/null")
		#echo $out
		if [ "$out" -ne "$dirMode" ]; then
			echo "Test$1: FAILED"
			return -1
		fi
		
		echo "Test$1: SUCCESS"
		return 0
	fi


	#echo "Checking whether output is correct"
	numDumpedFiles=$(ls "$dumpPath"  2>"/dev/null"| wc -l)
	#echo $numDumpedFiles
	if [ $numDumpedFiles -ne $numFiles ];
	then
		echo "Test$1: FAILED"
		return -1
	fi

	if [ $1 -eq 5 ]; then
		numFiles=$(expr $numFiles - 2)
	fi

	j=1
	while [ "$j" -le "$numFiles" ]; do
		origFile="${hiddenPath}/"
		origFile="$origFile$j"
		dumpedFile="${dumpPath}/"
		dumpedFile="$dumpedFile$j"
		#echo "$origFile"
		#echo "$dumpedFile"
		diff "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		if [ $? -ne 0 ];
		then
			echo "Test$1: FAILED"
			return -1
			
		fi
		j=$(($j + 1))
	done

	#diff additional files
	if [ $1 -eq 5 ]; then

		origFile="${hiddenPath}/"
		origFile="$origFile$3"
		dumpedFile="${dumpPath}/"
		dumpedFile="$dumpedFile$3"
		#echo "$origFile"
		#echo "$dumpedFile"
		diff "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		if [ $? -ne 0 ];
		then
			echo "Test$1: FAILED"
			return -1
			
		fi


		origFile="${hiddenPath}/"
		origFile="$origFile$Modifier$3"
		dumpedFile="${dumpPath}/"
		dumpedFile="$dumpedFile$Modifier$3"
		#echo "$origFile"
		#echo "$dumpedFile"
		diff "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		if [ $? -ne 0 ];
		then
			echo "Test$1: FAILED"
			return -1
			
		fi
	fi

	echo "Test$1: SUCCESS"
	return 0
}


testCD2 () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"

	tarName=$3
	dumpName="testDump"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	cd "$elfDir"

	touch "$errorPath"
	chmod -w "$curTestDir" 	
	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	error=$(cat "$creationErrorPath" 2>"/dev/null" | grep -i "$createError" 2>"/dev/null" | wc -l)
	chmod +w "$curTestDir" 
	if [ $error -eq 1 ]; then
		echo "Test$1: SUCCESS"
		return 0
	fi

	echo "Test$1: FAILED"
	return -1
}

 
testCD3 () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"

	tarName=$3
	dumpName="testDump"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	cd "$elfDir"

	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	#errorFileSize=$(stat -c %s $errorPath)


	touch "$errorPath"
	chmod -w "$curTestDir" 
	#echo "Extracting files from tar file"
	timeout 5m ./myTar -d "$tarPath" 1>"$errorPath" 2>&1
	chmod +w "$curTestDir" 

	error=$(cat "$errorPath" 2>"/dev/null"  | grep -i "$dumpError" 2>"/dev/null"  | wc -l)
	#echo $error
	if [ $error -eq 1 ]; then
		echo "Test$1: SUCCESS"
		return 0
	fi

	echo "Test$1: FAILED"
	return -1
}


testE () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"

	tarName=$3
	dumpName="IndividualDump"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	cd "$elfDir"

	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	#errorFileSize=$(stat -c %s $errorPath)


	#echo "Hiding original files"
	mkdir "$hiddenPath"
	j=1
	while [ "$j" -le "$numFiles" ]; do
		#echo $j
		origFile="${curTestDir}/"
		origFile="$origFile$j"
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		j=$(($j + 1))
	done

	
	if [ $4 -eq 0 ];then
		#echo "Extracting single non-existing file from tar file"
		timeout 5m ./myTar -e "$tarPath" "$4" 1>"$errorPath" 2>&1
		error=$(cat "$errorPath" 2>"/dev/null" | grep -i "$extractNoneError" 2>"/dev/null"  | wc -l)
		if [ $error -ne 1 ]; then
			echo "Test$1: FAILED"
			return -1
		fi


	elif [ $4 -eq 1 ];then
		#echo "Extracting single existing file from tar file"
		timeout 5m ./myTar -e "$tarPath" "$4" 1>"$errorPath" 2>&1

	elif [ $4 -eq 2 ];then
		#echo "Extracting two existing files from tar file"
		timeout 5m ./myTar -e "$tarPath" "1" 1>"$errorPath" 2>&1
		timeout 5m ./myTar -e "$tarPath" "2" 1>"$errorPath" 2>&1

	fi

	#echo "Checking whether output is correct"
	numDumpedFiles=$(ls "$dumpPath" 2>"/dev/null" | wc -l)
	if [ $numDumpedFiles -ne $4 ];
	then
		echo "Test$1: FAILED"
		return -1
	fi


	j=1
	while [ "$j" -le "$4" ]; do
		origFile="${hiddenPath}/"
		origFile="$origFile$j"
		dumpedFile="${dumpPath}/"
		dumpedFile="$dumpedFile$j"
		#echo "$origFile"
		#echo "$dumpedFile"
		diff "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		if [ $? -ne 0 ];
		then
			echo "Test$1: FAILED"
			return -1
			
		fi
		j=$(($j + 1))
	done

	echo "Test$1: SUCCESS"
	return 0
}


testL1 () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"


	tarName="$3"
	dumpName="tarStructure"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	rm -f "$errorPath"
	rm -f "$dumpName"
	#clean "$curTestDir"

	cd "$elfDir"


	#echo "Listing files from invalid tar file"
	timeout 1m ./myTar -l "$tarPath" 1>"$errorPath" 2>&1
	error=$(cat "$errorPath" 2>"/dev/null"  | grep -i "$listError" 2>"/dev/null" | wc -l)
	if [ $error -ne 1 ]; then
		echo "Test$1: FAILED"
		return -1
	fi


	echo "Test$1: SUCCESS"
	return 0
}

testL2 () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"


	tarName=$3
	dumpName="tarStructure"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	if [ -f "files.zip" ]; then
		unzip -q "files.zip"
		rm files.zip
	fi

	cd "$elfDir"

	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	#errorFileSize=$(stat -c %s $errorPath)


	#echo "Hiding original files"
	mkdir "$hiddenPath"

	if [ $1 -eq 14 ]; then
		origFile="${curTestDir}/abcdef1234567890"
		#echo $origFile
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
	else
	
		#j=1
		#while [ "$j" -le "$numFiles" ]; do
		for j in {a..x}{A..Y}{A..Y}{0..1}
		do
			#echo $j
			if [ $j == "xYY1" ]; then
				break	
			fi
			origFile="${curTestDir}/"
			origFile="$origFile$j"
			mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
			j=$(($j + 1))
		done

	fi


	#echo "Listing files from tar file"
	timeout 5m ./myTar -l "$tarPath" 1>"$errorPath" 2>&1

	#echo "Checking whether output is correct"
	genOutput="${curTestDir}/genOutput"
	expOutput="${curTestDir}/expOutput"
	outputLinesCount=$(expr $numFiles + 1)
	tail -n $outputLinesCount "$dumpPath" | sort > "$genOutput" 2>"/dev/null"
	diff "$genOutput" "$expOutput" 1>"/dev/null"  2>&1
	if [ $? -ne 0 ]; then
		echo "Test$1: FAILED"
		return -1 
	fi
	echo "Test$1: SUCCESS"
	return 0
}

testL3 () {
	local curTestDir="$curTestDir$1"
	#echo $curTestDir
	cd "$curTestDir"


	tarName=$3
	dumpName="tarStructure"
	errorName="other.error.log.test"
	errorName="$errorName$1"
	hiddenName="hiddenFiles"
	numFiles=$2
	
	tarPath="${curTestDir}/"
	dumpPath="$tarPath$dumpName"
	errorPath="$tarPath$errorName"
	hiddenPath="$tarPath$hiddenName"
	tarPath="$tarPath$tarName"
	creationErrorPath="${testsDir}/"
	creationErrorPath="${creationErrorPath}creation.error.log."
	creationErrorPath="${creationErrorPath}test"
	creationErrorPath="$creationErrorPath$1"

	#echo "Cleaning test dir"
	clean "$curTestDir"

	cd "$elfDir"
	#echo "$curTestDir"
	
	#echo "Creating tar file"
	timeout 5m ./myTar -c "$curTestDir" $tarName 1>"$creationErrorPath" 2>&1
	#errorFileSize=$(stat -c %s $errorPath)


	#echo "Hiding original files"
	mkdir "$hiddenPath"
	j=1
	while [ "$j" -le "$numFiles" ]; do
		#echo $j
		origFile="${curTestDir}/"
		origFile="$origFile$j"
		#echo "$origFile"
		#echo "$hiddenPath"
		mv "$origFile" "$hiddenPath" 1>"/dev/null" 2>&1
		j=$(($j + 1))
	done


	#echo "Listing files from tar file"
	touch "$errorPath"
	chmod -w "$curTestDir"
	timeout 5m ./myTar -l "$tarPath" 1>"$errorPath" 2>&1
	chmod +w "$curTestDir" 
	#echo "$errorPath"
	error=$(cat "$errorPath" 2>"/dev/null" | grep -i "$listError" 2>"/dev/null" | wc -l)
	#echo $error
	if [ $error -ne 1 ]; then
		echo "Test$1 FAILED"
		return -1
	fi

	echo "Test$1: SUCCESS"
	return 0
}

make clean > /dev/null
make > /dev/null

rm -rf tests
unzip -q tests.zip

############## creation and extraction ops ##############
#original sample test case shared. Create and extract 4 files. 
#2 marks
testCD 1 4 test.tar

#create and extract 30000 files (each file ranging from 1 byte to 1024 bytes)
#3 marks
testCD 2 30000 test.tar


#create and extract 10 files (each file is of 0 bytes) 
#2 marks
testCD 3  10 test.tar


#create and extract 5 files (each file is of 1GB). Thus, tar size will be ~5GB. 
#3 marks
testCD 4  5 test.tar

#recursive tar
#i.e. Create tar. Then again create tar. Then again create tar. Then untar
#2 marks 
testCD 5  4 test.tar

#No permission to write to test directory. Thus, tar file creation fails.
#Print error ”Failed to complete creation operation”
#2 marks
testCD2 6 4 test.tar


#No permission to create dump directory. Thus, tar file extraction fails.
#Print error ”Failed to complete extraction operation” 
#2 marks
testCD3 7 4 test.tar

#path to test directory is very long. create tar and extract files like test1. 
#3 marks
testCD 8 4 test.tar

#check whether output tar file and extracted files have correct permissions.
#1 mark
umask 0000
testCD 9 4 test.tar
umask 0022

############## single extraction ops ##############
#extract single file. Provide invalid filename for extraction.
#1 mark
#Print error ”Failed to complete extraction operation” 
testE 10 4 test.tar 0 


#extract single file. Provided valid filename for extraction.
#2 marks
testE 11 4 test.tar 1


#Perform extraction of a 2 files. Provided valid filenames for extraction.
#2 marks
testE 12 4 test.tar 2

############## list ops ##############
#Invalid tar file is present in the test directory. Invalid tar file means this tar file is not created using ./myTar -c.
#./myTar should be able to recognise that this is an invalid tar and can't do list operation on it.
#Print error ”Failed to complete list operation” 
#5 marks
testL1 13 4 test.tar 


#list contents of valid tar file
#2 marks
testL2 14 2 test.tar 


#Try to list contents of a valid tar file. No permissions to create tarStructure file in test directory.
#Print error ”Failed to complete list operation” 
#1 marks
testL3 15 4 test.tar 

#Do list operation on tar file containing 30000 files.
#2 marks
testL2 16 30000 test.tar 