#!/bin/bash         
#Χρησιμοποιούμε το bash

FAKELOS=multiplication_table
ARXEIO=mult_table_NUM.txt
ARITHMOS=1  #Ξεκινάμε από το 1 για να κανουμε λιγότερες πράξεις στην while που έχουμε μετά

if [ ! -d $FAKELOS ];
then
    mkdir $FAKELOS  #Φτιάχνουμε τον φάκελο.
fi
cd $FAKELOS
while [[ $ARITHMOS != 11 ]]; do
    METRITIS=1  #Ίδιος λόγος που ο ARITHMOS γίνεται 1
    ARXEIO_TWRA="./"$( echo $ARXEIO | sed s/NUM/$ARITHMOS/ )    #Το ./ δεν είναι απαραίτητο καθώς και στις 2 περιπτώσεις αναφερώμαστε στον ίδιο φάκελο
    if [ -e $ARXEIO_TWRA ];
    then
        rm $ARXEIO_TWRA     #Έγινε έλεγχος για ύπαρξη των αρχείων για να δούμε αν χρειάζεται να τα ξαναγράψουμε. Ο τρόπος που γράφουμε παρακάτω σχεδόν μας το επιβάλει αν θέλουμε κάθε φορά που τρέχουμε το πρόγραμμα να γίνονται διορθώσεις.
    fi
    while [[ $METRITIS != 11 ]]; do
        let APOTELESMA=$METRITIS*$ARITHMOS  #Μπορούσαμε και με APOTELESMA=$(( $METRITIS * $ARITHMOS )) αλλά προτήμισα την let. Δεν υπάρχει ουσιώδης λόγος γι'αυτή την επιλογή
        echo "$METRITIS x $ARITHMOS = $APOTELESMA" >> $ARXEIO_TWRA  #Κάνουμε append για να μπορούμε να έχουμε χαρακτήρα NEWLINE σε αντίθεση αν τα βάζαμε σε μια μεταβλητή συνολικά.
        let METRITIS=$METRITIS+1
    done
    let ARITHMOS=$ARITHMOS+1
done