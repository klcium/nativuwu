# NativUwU

Quick script / tutorial on how to dump these dex files.
For more information, see the `I DT_NEEDED HELP` article on my github blog.

This script is specific to the dji pilot version `1.15.4` and there are a lot of hardcoded values in my scripts.
My later self will hate me for that and so you will.

## Preparing the environment
**Requirements**: 
- A rooted Android Phone (easier)
- adb shell up and ready
- LIEF installed locally
- Be smart enought to update the file paths in my commands below to your context

### Install the APK
I used `adb install`. Then extract the libs from the APK `apktool d -s -r ./myfile.apk`

If you installed it from the Play Store, pull the `libAppGuard.so` file locally.


### Inject libAppGuard
The name of our lib is `libcaca.so`. 
So we will add it in the dependencies of the unpacker.

This is the "tricky" (annoying) part
- Use lief to inject your lib
- Fix the binary (`hexedit`) to make the dynamic segment and section entries have the same size.

I will test using an other lib than libappguard, if it worsk without manually patching the binary I will give out the script / cmd here.

### Preparing the so file

To compile:
```bash
cd memdumper
export NDK_PROJECT_PATH=$(pwd)/native
export APP_PLATFORM=android-21
$ANDROID_NDK_HOME/ndk-build 
```

Push the libs on `/data/local/tmp`
```bash
adb push ./native/obj/local/arm64-v8a/libcaca.so /data/local/tmp/
adb push ./libAppGuard.so /data/local/tmp/
```

In ADB, copy the libs in your data folder:
`cp /data/local/tmp/libAppGuard.so /app/data/someb64-appname/lib/your_arch/`
`cp /data/local/tmp/libcaca.so /app/data/someb64-appname/lib/your_arch/`


`mkdir /data/data/dji.go.v5/files/dumps`

### Pull the files
On the phone
```bash
## Via ADB
cd /data/data/dji.go.v5/files/
tar -xcf dumps.tar ./dumps/*.dump

## On the host
# Pull log files
adb pull /data/data/dji.go.v5/files/quoicoubeh.txt .
# Pull extracted data
adb pull /data/data/dji.go.v5/files/dumps.tar
# Extract them
tar -xvf ./dumps.tar

```

To find which one has your dex, just grep 
```
➜  patchtest grep DJIAccountAuthenticator ~/Downloads/caca/dump/dumps/*
grep: /home/owo/Downloads/caca/dump/dumps/7aea3d7000.dump: binary file matches
```

Then run the python script to extract the dex files:
```bash
# Edit the files
python3 ./extract_dex.py ./dumps/7aea3d7000.dump ./out
```

And voila you have your corrupted dex files :3
