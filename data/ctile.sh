cd tiles
mkdir ${2}
cd ${2}
rm *.png
wget ${1}
mv *.png ${2}1.png
convert ${2}1.png -scale 256x127 ${2}0.png
convert ${2}1.png -scale 64x31 ${2}2.png
echo "${2}0.png Z0 X0 Y0" > info
echo "${2}1.png Z1 X0 Y0" >> info
echo "${2}2.png Z2 X0 Y0" >> info
cd ../..

