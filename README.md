# This is code of the algorithms for the anchored community search problem

## data type

edge list

## compile

cmake -S ./

## run

bin/adsRunner : LA in c++.

bin/betalocalDensity : FDP

bin/exactLocalDensity : FDPE

bin/flowseedsRunner : FS in c++

### parameters
-R, -A, -f_txt (or noUVM -f_txt), -T

```bin/flowseedsRunner -f_txt data/lj/data.txt -R data/lj/seeds/464_704551_24_10.R -A data/lj/seeds/464_704551_24_10.A ```