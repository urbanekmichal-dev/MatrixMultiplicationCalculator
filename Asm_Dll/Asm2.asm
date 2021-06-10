.DATA
data_size QWORD 4
rozmiar_wektora QWORD 4
zero QWORD 0
jeden QWORD 1
macierzA QWORD ?
macierzB QWORD ?
macierzC QWORD ?
.CODE

MNOZENIE_ASM proc
	;rcx - macierzA, nast�pnie przechowywany licznik, kt�ry pozwala na sprawdzenie czy nale�y wykona� mno�enie wektor�w
	;rdx - macierzB
	;r8- macierzC
	;r9 - m - liczba wierszy w macierzy A
	;r10 - n - liczba kolumn w macierzy A
	;r11 - p - liczba kolumn w macierzy B
	;r12 - i
	;r13 - j
	;r14 - k
	;rdi - kom�rka macierzy A
	;rsi - komorka macierzy B
	;r15 - pocz�tkowo inicjalizacji zmiennej i warto�ci� x, nast�pnie uzywana jako zmienna s
	;xmm0 - pomocniczy, wprowadzane sa do niego dane
	;xmm1 - przechowywane warto�ci kom�rek macierzy A, a nst�pnie wynik mno�enia
	;xmm2 - przechowywane warto�ci kom�rek macierzy B

	push rbx						;zapis na stos rejest�w, kt�rych warto�� musi zosta� odtworzona
	push rsi
	push rdi	
	push r12	
	push r13	
	push r14
	push r15
	mov r10d, DWORD PTR[rsp +96]	;pi�ty argument, pobrany ze stosu
	mov r11d, DWORD PTR[rsp +104]	;sz�sty argument, pobrany ze stosu
	mov r15d, DWORD PTR[rsp +112]
	;


	mov macierzA, rcx				;zapisanie przekazanych adres�w tablic w odpowiednich zmiennych
	mov macierzB, rdx
	mov macierzC, r8
	mov rax, 0
	mov r12, r15					;wpisanie do i warto�ci x			
	mov r13, rax					;wyzerowanie licznik�w j oraz k
	mov r14, rax
	movq xmm1, zero					;zerowanie wektora kom�rek pamieci A
	movq xmm2, zero					;zerowanie wektora kom�rek pamieci B
Petla_i:
	mov rax, r9						;obs�uga wyj�� z p�tli
	cmp r12, rax					
	jge Koniec_i
Petla_j:
	mov rax, r11			
	cmp r13, rax				
	jge Koniec_j
	
	mov r15, 0						;wyzerowanie zmiennej s
	mov rcx, 1						;ustawenie licznika aktualnego numeru liczb wprowadzonych do wektor�w xmm1 oraz xmm2 powi�kszona o 1
Petla_k:
	mov rax, r10
	cmp r14, rax					
	jl Start						;skok na pocz�tek najbardziej wewn�rzn�j p�tli
	mov rax, jeden					;sprawdzenie czy wektory xmm1 oraz xmm2 s� puste
	cmp rcx, rax					
	jne Mnozenie_wektorow			;je�eli tak, to skok do wykonania mno�enia wektor�w
	jmp Koniec_k

Start:	
	mov rbx, macierzA				;przes?anie adresu macierzy A do rejestru rbx
	mov rax, r12					;przes?anie do rejestru rax zmiennej i
	mul r10							;pomno?enie warto?ci indeksu i przez rozmiar wiersza w macierzy A
	add rax, r14					;dodanie do obliczjedenj warto?ci, warto?ci zmiennej k
									;powy?sze trzy rozkazy s?u?y?y obliczeniu indeksu kom�rki w macierzy,
									;ktora jest przechowywana jako tablica jednowymiarowa.
	mul data_size					;pomno?enie indeksu w tabliczy przez 4, poniewa? obliczona wy?ej warto?? jest w bajtach, 
									;a potrzebujemy dosta? si? do inta o rozmiarze 4.
	
	MOVD xmm0, dword ptr [rbx+rax]	;wpisanie do wektora kom�rki z macierzy A
	PSLLDQ xmm1, 4					;przesuni?cie danych na nast?pne pozycje
	PADDD xmm1, xmm0				;dodanie nowej warto?ci do zapisanych w wektorze xmm1

	mov rbx, macierzB				;to samo co wy?ej tylko dla macierzy B, u?ywaj?c odpowiednich - innych zmiennych
	mov rax, r14
	mul r11
	add rax, r13
	mul data_size

	MOVD xmm0, dword ptr [rbx+rax]	;wpisanie do wektora kom�rki z macierzy B
	PSLLDQ xmm2, 4					;przesuni?cie danych na nast?pn? pozycje
	PADDD xmm2, xmm0				;dodanie nowej warto?ci do zapisanych w wektorze xmm2
	
	mov rax, rozmiar_wektora		;sprawdzenie czy wektory s? pe?ne i nale?y wykona? mno?enie		
	cmp rcx, rax					
	jl No_multiply					;skok na koniec petli, je?eli nie trzeba wykona? mno?enia

Mnozenie_wektorow:
	PMULLD xmm1, xmm2				;mno?enie odpowiednich kom�rek
	PHADDD xmm1, xmm1				;sumowanie kom�rek w wektorze xmm1
	PHADDD xmm1, xmm1				;potrzebane powt�rne wykonanie sumowania, 
									;poniewa? ka?dy taki rozkaz sumuje dwie z czterech liczb w nim
	movd edx, xmm1					;przepisanie sumy iloczyn�w kom�rek macierzy A i B, do rejestru rdx
	add r15, rdx					;dodanie do zmiennej s wyliczjedenj warto?ci
	movq xmm1, zero					;wyzerowanie rejestru xmm1
	movq xmm2, zero					;wyzerowanie rejestru xmm2
	mov rcx, 0						;wyzerowanie rejestru rcx

No_multiply:
	inc rcx							;inkrementacja numeru liczb w wektorach xmm1 oraz xmm2
	inc r14							;inkrementacja k
	jmp Petla_k						
Koniec_k:
	mov rbx, macierzC				;wyliczenie adresu odpowiedniej kom�rki w  macierzy C, analogicznie do A i B
	mov rax, r12
	mul r11
	add rax, r13
	mul data_size
				
	mov dword ptr [rbx+rax], r15d	;zapis wyniku do komorki macierzy C

	mov r14, 0						;zerowanie k	
	inc r13							;inkrementacja j						
	jmp Petla_j						
Koniec_j:
	mov r13, 0						;zerowanie j		
	inc r12							;inkrementacja i		
	jmp Petla_i						
Koniec_i:
	pop r15							;pobranie warto�ci ze stosu, w celu odtworzenia warto�ci rejestr�w
	pop r14						
	pop r13					
	pop r12						
	pop rdi
	pop rsi
	pop rbx
	ret

MNOZENIE_ASM endp

END