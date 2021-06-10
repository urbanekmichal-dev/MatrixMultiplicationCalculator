
.DATA
i DWORD ?
j DWORD ?
k DWORD ?
eight DWORD 8
a_value real8 ?
b_value real8 ?

.code
MNOZENIE_ASM proc matrixA : DWORD, matrixB : DWORD, matrixC : DWORD, n: DWORD, m: DWORD, p: DWORD
	
	mov eax, 0 ; zerowanie licznika
	mov i, eax
	mov j, eax
	mov k, eax
Loop_j:			;obsługa petli
	mov eax, p				;przes³anie zawartoœci argumentu do akumulatora
	cmp j, eax					;porównanie licznika zewnêtrznego z wysokoœci¹
	jge End_j
Loop_k:
	mov eax, p				;przes³anie zawartoœci argumentu do akumulatora
	cmp k, eax					;porównanie licznika zewnêtrznego z wysokoœci¹
	jge End_k
Loop_i:
	mov eax, p				;przes³anie zawartoœci argumentu do akumulatora
	cmp i, eax					;porównanie licznika zewnêtrznego z wysokoœci¹
	jge End_i
	
	mov ebx, matrixA ;przesłanie elementu tablicy do rejestru
	mov eax, i ;przesłanie wartosci zmiennej i do rejestru
	mul m
	add eax, k
	mul eight
	;mov a_value, [ebx + eax]
	fld qword ptr [ebx+eax]		;³adowanie do rejestru jednostki zmiennoprzecinkowej
	
	mov ebx, matrixB
	mov eax, k
	mul p
	add eax, j
	mul eight
	fld qword ptr [ebx+eax]		
	
	fmulp ST(1), ST(0)
	
	mov ebx, matrixC
	mov eax, i
	mul p
	add eax, j
	mul eight
	fld qword ptr [ebx+eax]	
	
	faddp ST(1), ST(0)
	fstp qword ptr [ebx+eax]	
	
	inc i
	jmp Loop_i					;skok na pocz¹tek wewnêtrznej pêtli
End_i:
	mov i, 0					;zerowanie licznika wewnêtrznego
	inc k						;inkrementacja licznika
	jmp Loop_k					;skok na pocz¹tek zewnêtrznej pêtli
End_k:
	mov k, 0					;zerowanie licznika wewnêtrznego
	inc j						;inkrementacja licznika
	jmp Loop_j					;skok na pocz¹tek zewnêtrznej pêtli
End_j:
	ret							;powrót z podprogramu

MNOZENIE_ASM endp
end

