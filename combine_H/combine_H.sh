#/bin/sh
declare -i i=1
declare -i j=1
declare -i nvars=1
declare -i ncoeff_out=1
declare -i ncoeff1=1
read -p "Give the filename of the first subproblem H matrix : " filename1
echo $filename1
read -p "Give the output filename for the merged H matrix : " filename2
echo $filename2
cp $filename1 $filename2
nvars=`wc -l $filename2| awk '{print $1}'| bc`
echo "Number of variables : $nvars"
echo "$nvars 1" > neutral_"$nvars"vars.vec
while [[ $i -le $nvars ]]
do 
    read -p "Give the filename of the following subproblem H matrix [if finish, just hit enter] : " filename1
    if [[ ! -f $filename1 ]]
    then
	break 2
    fi
    j=1
    while [[ $j -le $nvars ]]
    do
	ncoeff1=`head -n $j $filename1 | tail -n 1 | wc -w |awk '{print $1}'|bc`
	ncoeff_out=`head -n $j $filename2 | tail -n 1 | wc -w |awk '{print $1}'|bc`
	if [[ $ncoeff1 -gt $ncoeff_out ]]
	then
	    echo "$j s|^.*$|" > cmd.sed
	    tr -d '\n' < cmd.sed > cmd.sed2
	    head -n $j $filename1 | tail -n 1 >> cmd.sed2
	    tr -d '\n' < cmd.sed2 > cmd.sed
	    echo "|" >> cmd.sed
	    tr -d '\n' < cmd.sed > cmd.sed2
	    sed -f cmd.sed2 $filename2 > $filename2.temp
	    mv $filename2.temp $filename2
	    rm cmd.sed
	    rm cmd.sed2
	fi
	j='j+1'
    done
done
