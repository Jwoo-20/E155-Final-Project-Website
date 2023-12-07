/*
James Woo, jmwa2020@mymail.pomona.edu
Kaito Robson, kara2020@mymail.pomona.edu
12/7/2023
Reads keypad inputs and checks for codes
*/

// synchronizer module: to mitigate metastability since the button presses on the keypad are asynchronous
module synchronizer(
    input  logic       clk,
    input  logic [3:0] d,
    output logic [3:0] q
);
    //4-bit synchronizer since there are 4 rows in the 4x4 matrix keypad
    logic [3:0] buffer_1; 
	logic [3:0] buffer_2;
	
	// two stages of buffers
    always_ff @(posedge clk)
        begin
            buffer_1 <= d;
            buffer_2 <= buffer_1;
            q <= buffer_2;
        end
endmodule


// keypad_scanner module: actively scans the columns, waiting for a button press, which 
// determines the row. This will then allow for the exact key pressed to be identified
// the output binary_hex_value is determined from the value shown on the keypad itself
module keypad_scanner(
    input  logic 	   clk, 
	input  logic 	   reset,
    input  logic [3:0] row,
	output logic       en,
    output logic [3:0] col, 
	output logic [3:0] binary_hex_value
);
	// one-hot encoding for the state machine
	typedef enum logic [23:0]{
	S0 =  24'b000000000000000000000001,
	S1 =  24'b000000000000000000000010, 
	S2 =  24'b000000000000000000000100, 
	S3 =  24'b000000000000000000001000, 
	S4 =  24'b000000000000000000010000, 
	S5 =  24'b000000000000000000100000, 
	S6 =  24'b000000000000000001000000, 
	S7 =  24'b000000000000000010000000, 
	S8 =  24'b000000000000000100000000, 
	S9 =  24'b000000000000001000000000, 
	S10 = 24'b000000000000010000000000, 
	S11 = 24'b000000000000100000000000, 
	S12 = 24'b000000000001000000000000, 
	S13 = 24'b000000000010000000000000, 
	S14 = 24'b000000000100000000000000, 
	S15 = 24'b000000001000000000000000, 
	S16 = 24'b000000010000000000000000, 
	S17 = 24'b000000100000000000000000, 
	S18 = 24'b000001000000000000000000, 
	S19 = 24'b000010000000000000000000,
	S20 = 24'b000100000000000000000000, 
	S21 = 24'b001000000000000000000000, 
	S22 = 24'b010000000000000000000000, 
	S23 = 24'b100000000000000000000000
	} statetype;
	
    statetype state, nextstate;

    always_ff @ (posedge clk)
        if (reset == 0) state <= S1;
        else state <= nextstate;
    
    //nextstate logic
    always_comb
        case (state)
            S1: 
				if (row == 4'b1110) nextstate = S2;
                else if (row == 4'b1101) nextstate = S3;
                else if (row == 4'b1011) nextstate = S4;
                else if (row == 4'b0111) nextstate = S5;
                else nextstate = S6;

                S2: nextstate = S0;
                S3: nextstate = S0;
                S4: nextstate = S0;
                S5: nextstate = S0;

            S6: 
				if (row == 4'b1110) nextstate = S7;
                else if (row == 4'b1101) nextstate = S8;
                else if (row == 4'b1011) nextstate = S9;
                else if (row == 4'b0111) nextstate = S10;
                else nextstate = S11;

                S7: nextstate = S21;
                S8: nextstate = S21;
                S9: nextstate = S21;
                S10: nextstate = S21;
                
            S11: 
				 if (row == 4'b1110) nextstate = S12;
                 else if (row == 4'b1101) nextstate = S13;
                 else if (row == 4'b1011) nextstate = S14;
                 else if (row == 4'b0111) nextstate = S15;
                 else nextstate = S16;

                 S12: nextstate = S22;
                 S13: nextstate = S22;
                 S14: nextstate = S22;
                 S15: nextstate = S22;

            S16: 
				 if (row == 4'b1110) nextstate = S17;
                 else if (row == 4'b1101) nextstate = S18;
                 else if (row == 4'b1011) nextstate = S19;
                 else if (row == 4'b0111) nextstate = S20;
                 else nextstate = S1;

                 S17: nextstate = S23;
                 S18: nextstate = S23;
                 S19: nextstate = S23;
                 S20: nextstate = S23;

            S0: if (row == 4'b1111) nextstate = S1;
                else nextstate = S0;
					
            S21: if (row == 4'b1111) nextstate = S1;
                 else nextstate = S21;
					 
            S22: if (row == 4'b1111) nextstate = S1;
                 else nextstate = S22;
					 
            S23: if (row == 4'b1111) nextstate = S1;
                 else nextstate = S23;
					 
            default: nextstate = S1;
    endcase
    //output logic for col (columns)
    always_comb
        case(state)
            S1:  col = 4'b1101;
            S2:  col = 4'b1101;
            S3:  col = 4'b1101;
            S4:  col = 4'b1101;
            S5:  col = 4'b1101;
            S0:  col = 4'b0000;
            S6:  col = 4'b1011;
            S7:  col = 4'b1011;
            S8:  col = 4'b1011;
            S9:  col = 4'b1011;
            S10: col = 4'b1011;
            S21: col = 4'b0000;
            S11: col = 4'b0111;
            S12: col = 4'b0111;
            S13: col = 4'b0111;
            S14: col = 4'b0111;
            S15: col = 4'b0111;
            S22: col = 4'b0000;
            S16: col = 4'b1110;
            S17: col = 4'b1110;
            S18: col = 4'b1110;
            S19: col = 4'b1110;
            S20: col = 4'b1110;
            S23: col = 4'b0000;
            default: col = 4'b1111;
        endcase
    //output logic for binary_hex_value
        always_comb
            case (state)
                S2:  binary_hex_value = 4'b0010; //2
                S3:  binary_hex_value = 4'b0101; //5
                S4:  binary_hex_value = 4'b1000; //8
                S5:  binary_hex_value = 4'b0000; //0
                S7:  binary_hex_value = 4'b0011; //3
                S8:  binary_hex_value = 4'b0110; //6
                S9:  binary_hex_value = 4'b1001; //9
                S10: binary_hex_value = 4'b1111; //F
                S12: binary_hex_value = 4'b1010; //A
                S13: binary_hex_value = 4'b1011; //b
                S14: binary_hex_value = 4'b1100; //C
                S15: binary_hex_value = 4'b1101; //d
                S17: binary_hex_value = 4'b0001; //1
                S18: binary_hex_value = 4'b0100; //4
                S19: binary_hex_value = 4'b0111; //7
                S20: binary_hex_value = 4'b1110; //E
                default: binary_hex_value = 4'b0000;
            endcase
    //output logic for en (enable)
        always_comb
            case (state)
                S2:  en = 1;
                S3:  en = 1;
                S4:  en = 1;
                S5:  en = 1;
                S7:  en = 1;
                S8:  en = 1;
                S9:  en = 1;
                S10: en = 1;
                S12: en = 1;
                S13: en = 1;
                S14: en = 1;
                S15: en = 1;
                S17: en = 1;
                S18: en = 1;
                S19: en = 1;
                S20: en = 1;
                default: en = 0;
            endcase
endmodule


// hex_digit_display module: stores the current and previous keypress (defaulting at zero). 
// After the second keypress, shifts the first keypress to the left and displays last keypress on the right.
module hex_digit(
    input  logic       clk, 
	input  logic       reset, 
	input  logic       en,
    input  logic [3:0] binary_hex_number,
	output logic enab, disab, easter_egg_code
);
    
	logic [3:0] seg_a, seg_b, seg_c, seg_d, seg_e;
	
    always_ff @ (posedge clk)
        if (reset == 0) 
            begin
				seg_a <= 4'b0000;
				seg_b <= 4'b0000;
				seg_c <= 4'b0000;
				seg_d <= 4'b0000;
				seg_e <= 4'b0000;
            end

		//stores last 5 inputs
        else if (en) 
            begin
				seg_e <= seg_d;
				seg_d <= seg_c;
				seg_c <= seg_b;
                seg_b <= seg_a;
                seg_a <= binary_hex_number;
            end
				
			//checks to see if the most recent 5 inputs match any of the recognized codes
			assign enab = ((seg_e == 4'b0000) & (seg_d == 4'b0001) & (seg_c == 4'b0010) & (seg_b == 4'b0011) & (seg_a == 4'b1111));
			assign disab = ((seg_e == 4'b1000) & (seg_d == 4'b1001) & (seg_c == 4'b1010) & (seg_b == 4'b1011) & (seg_a == 4'b1111));
			assign easter_egg_code = ((seg_e == 4'b0101) & (seg_d == 4'b0010) & (seg_c == 4'b0100) & (seg_b == 4'b0110) & (seg_a == 4'b1111));
					
endmodule




//checks to see if a valid code was input and stores that until another code is input
module dig_check(
	input logic clk, reset, enab_code, disab_code,
	output logic enab, disab
	);
	
	typedef enum logic [1:0] {S0, S1} statetype;
	
	statetype state, nextstate;
	
		always_ff @ (posedge clk)
		if (reset == 0) state <= S0;
		else state <= nextstate;
	
	//if disabled and enable code is input, set to enable until disable code is input
	always_comb
		case (state)
			S0: if (enab_code) nextstate = S1;
				else nextstate = S0;
			S1: if (disab_code) nextstate = S0;
				else nextstate = S1;
			default: nextstate = S0;
		endcase
		
		assign enab = (state == S1);
		assign disab = (state == S0);
		
	endmodule
	
	
