# paint thing

a paint program written in C using SDL3<br>
<br>
it also uses xz to compress the font image, might use it for more later on if I want to add in more textures<br>

you should just be able to run `./build.sh` from this directory and it should download and compile both SDL3 and xz (xz is statically linked with the program but SDL3 is dynamically linked)<br>
if you're compiling this on windows or cross compiling to it, you can change the CC variable in build.sh to the mingw compiler and it should still work (though if you compiled for linux previously you will probably need to remove the previously compiled SDL3 and xz stuff)
