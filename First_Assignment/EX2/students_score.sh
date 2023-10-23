#!/bin/bash

FAKELOS_ELEGXOU=$1              #Κάνουμε τις αρχηκοποιήσεις μας
FAKELOS_EKTELESIS=$(pwd)
YPOFAKELOI="student_NUM"
ARXEIA_PYGES="homeworkNUM.c"    #Mπορούσαμε να κάνουμε κάτι με το * αλλά δεν είχα όρεξη
ARXEIA_REPORTS="report.txt"
ARXEIA_SCORES="score.txt"
FAKELOS_APOTHIKEFSHS="folder_organized"
MATHITES=10
ASKISEIS_RESULTS=( 30 60 90 )
ASKISEIS_PERCENT=( 20 30 50 )
METRITIS_1=0

if [ -z $1 ]; then
    echo -n "Please specify the folder you want this program to work on: "
    read EISODOS
    FAKELOS_ELEGXOU=$EISODOS
fi
cd $FAKELOS_ELEGXOU
FAKELOS_ELEGXOU_TRUE=$(pwd)
cd $FAKELOS_EKTELESIS
if [ ! -d $FAKELOS_ELEGXOU ]; then
    echo "Given folder doesn't exist."
    exit 1                                  #Είπα να βάλω return values γιατί... γιατί οχι;
fi
if [ -e $ARXEIA_SCORES ]; then
        rm $ARXEIA_SCORES
    fi
cd $FAKELOS_ELEGXOU
while [[ $METRITIS_1 != $MATHITES ]]; do    #Βρόγχος για κάθε μαθητή

    MET_1=$(( $METRITIS_1 + 1 ))
    if [ ! -d "./"$( echo $YPOFAKELOI | sed s/NUM/$MET_1/ ) ]; then
        echo "Invalid folder format. Stoping testing. Some folders may have results."
        exit 2
    fi
    cd $( echo $YPOFAKELOI | sed s/NUM/$MET_1/ )

    if [ ! -e $ARXEIA_REPORTS ]; then
        echo "No report found on folder "$(pwd)
        exit 3
    fi
    ENDIAMESOS=( $(cat $ARXEIA_REPORTS) )
    MATHITES_ONOMA[$METRITIS_1]=${ENDIAMESOS[0]}
    MATHITES_AM[$METRITIS_1]=${ENDIAMESOS[1]}
    let MATHITES_BATHMOI[$METRITIS_1]=0
    let METRITIS_2=0
    echo -n "${MATHITES_ONOMA[$METRITIS_1]} ${MATHITES_AM[$METRITIS_1]}" >> $FAKELOS_EKTELESIS/$ARXEIA_SCORES
    while [[ $METRITIS_2 != ${#ASKISEIS_RESULTS[@]} ]]; do  #Βρόγχος για κάθε άσκηση. Μπορούμε να χρησιμοποιήσουμε το μέγεθος του πίνακα για να μάθουμε πόσες ασκήσεις έχει να κάνει ο κάθε μαθητής.
        MET_2=$(( $METRITIS_2 + 1 ))
        if [ ! -e "./"$( echo $ARXEIA_PYGES | sed s/NUM/$MET_2/ ) ]; then
            echo "No source code file with the correct name found in "$(pwd)
            exit 4                          #Μπορούσαμε να βαθμολογίσουμε τον μαθητή με 0 εδώ αλλά καλό είναι να είμαστε λίγο επιεικείς :)
        fi
        gcc $( echo $ARXEIA_PYGES | sed s/NUM/$MET_2/ )
        if [[ $? != 0 ]]; then
            echo "Program failed to compile."
        else
            if [ $(./a.out) = ${ASKISEIS_RESULTS[$METRITIS_2]} ]; then
                let MATHITES_BATHMOI[$METRITIS_1]=${MATHITES_BATHMOI[$METRITIS_1]}+${ASKISEIS_PERCENT[$METRITIS_2]}
                echo -n " $( echo -n "homeworkNUM:" | sed s/NUM/$MET_2/ ) ${ASKISEIS_PERCENT[$METRITIS_2]}" >> $FAKELOS_EKTELESIS/$ARXEIA_SCORES
            else
                echo -n " $( echo -n "homeworkNUM:" | sed s/NUM/$MET_2/ ) 0" >> $FAKELOS_EKTELESIS/$ARXEIA_SCORES
            fi
            rm ./a.out
        fi
        METRITIS_2=$(( $METRITIS_2 + 1 ))
    done
    echo " total_grade: ${MATHITES_BATHMOI[$METRITIS_1]}" >> $FAKELOS_EKTELESIS/$ARXEIA_SCORES

    cd ..
    METRITIS_1=$(( $METRITIS_1 + 1 ))
done

let MAX_BATHMOS=0
let MIN_BATHMOS=100
let EPITYXONTES=0
let METRITIS_1=0
while [[ $METRITIS_1 != $MATHITES ]]; do

    if [ $MAX_BATHMOS -lt ${MATHITES_BATHMOI[$METRITIS_1]} ]; then
        let MAX_BATHMOS=${MATHITES_BATHMOI[$METRITIS_1]}
    fi
    if [ $MIN_BATHMOS -gt ${MATHITES_BATHMOI[$METRITIS_1]} ]; then
        let MIN_BATHMOS=${MATHITES_BATHMOI[$METRITIS_1]}
    fi
    if [ ${MATHITES_BATHMOI[$METRITIS_1]} -ge 50 ]; then
        let EPITYXONTES=$EPITYXONTES+1
    fi
    METRITIS_1=$(( $METRITIS_1 + 1 ))
done

echo "Successful students: $EPITYXONTES"
echo "Maximum grade achived: $MAX_BATHMOS"
echo "Minimum grade achived: $MIN_BATHMOS"

echo -n "Would you like to organize your files in a folder? [y/N]:"
read EISODOS
if [ ! "$EISODOS" == "y" ] && [ ! "$EISODOS" == "Y" ]; then
    #echo "Goodbye!"
    exit 0
fi

cd $FAKELOS_EKTELESIS

if [ -d $FAKELOS_APOTHIKEFSHS ]; then
    rm -r $FAKELOS_APOTHIKEFSHS
fi
mkdir $FAKELOS_APOTHIKEFSHS
cd $FAKELOS_APOTHIKEFSHS

METRITIS_1=0
while [[ $METRITIS_1 != $MATHITES ]]; do
    MET_1=$(( $METRITIS_1 + 1 ))

    METRITIS_2=0
    while [[ $METRITIS_2 !=  ${#ASKISEIS_RESULTS[@]} ]]; do
        MET_2=$(( $METRITIS_2 + 1 ))

        cp $FAKELOS_ELEGXOU_TRUE/$( echo $YPOFAKELOI | sed s/NUM/$MET_1/ )/$( echo $ARXEIA_PYGES | sed s/NUM/$MET_2/ ) ./$( echo "${MATHITES_ONOMA[$METRITIS_1]}_${MATHITES_AM[$METRITIS_1]}_homeworkNUM.c" | sed s/NUM/$MET_2/ )

        METRITIS_2=$(( $METRITIS_2 + 1 ))
    done

    METRITIS_1=$(( $METRITIS_1 + 1 ))
done
