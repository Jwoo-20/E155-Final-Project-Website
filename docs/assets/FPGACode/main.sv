/*
James Woo, jmwa2020@mymail.pomona.edu
Kaito Robson, kara2020@mymail.pomona.edu
12/7/2023
Reads inputs from a 4x4 keypad matrix. Detects 3 different codes that either enable or disable the system functionality
Different codes show different display animations on 8x8 LED matrix. System remains enabled/disaled until correct disable/enable
code turns it on/off. 
*/



// top module: calls modules to read keypad inputs and display led matrix output
module top(
	input  logic       reset, helmet_open,
    input  logic [3:0] r,
    output logic [3:0] c,
	output logic enab, eyes,
	output logic [7:0] row, col
);    

    // Internal high-speed oscillator, 24MHz
    HSOSC #(.CLKHF_DIV(2'b01))
        hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(clk));

	// 16-bit counter
    logic [15:0] counter;
    logic low_speed_clock;
	
	//clock divider
    always_ff @(posedge clk) 
        begin
            if (reset == 0) counter <= 0;
            else counter <= counter + 1;
            low_speed_clock <= counter[15];
        end

    logic [3:0] binary_hex_number;
    logic [3:0] seg_a, seg_b, seg_c, seg_d, seg_e;
    logic [3:0] hex_display_out; 
    logic [3:0] synchronized_row;  // the intended output of the instantiation of the sychronizer module to mitigate metastability
	logic [7:0] rowc, colc;

	// instantiating synchronizer module
    synchronizer synchronize(clk, r, synchronized_row);

	// instantiating keypad_scanner module
    keypad_scanner key_scan(low_speed_clock, reset, synchronized_row, en, c, binary_hex_number); 

	// instantiating hex_digit_display module
    hex_digit hex_dig(low_speed_clock, reset, en, binary_hex_number, enab_code, disab_code, easter_egg_code);

	//calculates the pattern to be displayed on the led matrix
	led_outputs lo(clk, reset, enabon, disab, eegg, rowc, colc, leyes);
	assign row = helmet_open ? 8'b11111111 : rowc;
	assign col = helmet_open ? 8'b11111111 : colc;
	assign eyes = helmet_open ? 1'b0 : leyes;


	//logic for enabling and disabling the system
	assign enab_code_en = (enab_code & disab);
	assign easter_egg_code_en = (easter_egg_code & disab);
	assign disab = (disab1 & disab2);
	assign enab = (enabon | eegg);
	
	//checking what the most recent code input was
	dig_check onoff(low_speed_clock, reset, enab_code_en, disab_code, enabon, disab1);
	dig_check eggoff(low_speed_clock, reset, easter_egg_code_en, disab_code, eegg, disab2);
		

	
endmodule


