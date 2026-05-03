# replace "secret" with a password below
# run this to build two headless client folders and launch them both
# in nohup mode

pw="secret"
portA=5077
portB=5078



cd /home/jcr13/checkout/CastleDoctrine/gameSource
./makeHeadlessGame.sh
./makeHeadlessGameFolder.sh


cd /home/jcr13

# kill existing, if running

echo -n "quit\n$pw\n[END_REQUEST]" | netcat localhost $portA
echo -n "quit\n$pw\n[END_REQUEST]" | netcat localhost $portB


rm -r headlessClientA/
rm -r headlessClientB/

cp -r checkout/CastleDoctrine/gameSource/headlessFolder ./headlessClientA

echo $pw > ./headlessClientA/settings/simulatorServerPassword.ini

echo $portA > ./headlessClientA/settings/simulatorServerPort.ini

cp -r ./headlessClientA ./headlessClientB

echo $porB > ./headlessClientB/settings/simulatorServerPort.ini

cd ./headlessClientA
nohup ./CastleDoctrineHeadless > headlessLog.txt 2>&1 &


cd ../headlessClientB
nohup ./CastleDoctrineHeadless > headlessLog.txt 2>&1 &

