/*
Write an Odd-Even Sort using different processes (not programs- you must use fork()).  
 
This sort compares two adjacent numbers and switches them, if the first number is greater than the second 
number to get an ascending order list. The opposite case applies for a descending order series. Odd-Even 
transposition sort operates in two phases − odd phase and even phase. In both the phases, processes 
exchange numbers with their adjacent number in the right.

procedure ODD-EVEN_PAR (n) 

begin 
   id := process's label 
	
   for i := 1 to n do 
   begin 
	
      if i is odd and id is odd then 
         compare-exchange_min(id + 1); 
      else 
         compare-exchange_max(id - 1);
			
      if i is even and id is even then 
         compare-exchange_min(id + 1); 
      else 
         compare-exchange_max(id - 1);
			
   end for
	
end ODD-EVEN_PAR


IF ARRAY LENGTH EVEN START WITH ODD PHASE
ELSE IF ARRAY LENGTH IS ODD START WITH EVEN PHASE




8 9_2 4_7 EVEN
|
8_2 9_7 4 ODD
        |
2 8_7 9_4 EVEN
|
2_7 8_4 9 ODD
        |
2 7_4 8_9 EVEN
|
2 4 7 8 9 DONE

https://formal.kastel.kit.edu/ulbrich/verifythis2017/challenge3.pdf
*/