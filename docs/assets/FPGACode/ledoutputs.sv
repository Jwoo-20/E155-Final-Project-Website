//displays led animation based on inputs from the keypad	
module led_outputs(
     input logic clk, reset, enab, disab, eegg,
	 output logic [7:0] row, col,
	 output logic eyes
	);

   logic clkslow;
   logic [4:0] frame;
   logic [28:0] counter;
   logic [63:0] doton, dotoff, dotegg, dot;
   logic [7:0] rowon, colon, rowoff, coloff, roweg, coleg;
   
  
   //clock divider
   always_ff @(posedge clk) begin
     if(reset == 0 | reset_frames) begin 
		 counter <= 0;
		 end
     else  begin    
		 counter <= counter + 1; 
   end
   end
  
  
assign clkslow = counter[23];

//logic for which frame to display
frames f(clk, reset, reset_frames, counter[27:23], frame);  
restart_frames rf(clk, reset, enab, disab, eegg, reset_frames);
  
//calculating the outputs  
on_disp ond(frame, doton);
off_disp ofd(frame, dotoff);
easteregg_disp eggd(frame, dotegg); 

led_tmux ltmegg(counter[14], reset, counter[16:14], dot, row, col);

//pick which output to display
out_picker op(doton, dotoff, dotegg, enab, disab, eegg, dot);
assign eyes = (dot ==  64'b1111111111111111111111111111111111111111111111111111111111111111);
  
endmodule


//picks which display to output based on the current code
module out_picker(input logic [63:0] doton, dotoff, dotegg,
	input logic enab, disab, eegg,
	output logic [63:0] dot);
	
	
	always_comb
		if(enab) begin
			dot <= doton;
			end
		else if (disab) begin
			dot <= dotoff;
			end
		else if (eegg) begin
			dot <= dotegg;
		end
		else begin
			dot <= 0;
		end
	
	
endmodule


//logic for when to start animation from frame 0
module restart_frames(input logic clk, reset, enab, disab, eegg,
	output logic reset_frames);
	
	logic [2:0] sa, sb;
	
	always_ff @ (posedge clk)
	begin
		if (reset == 0) begin
			sa <= 3'b000;
			sb <= 3'b000;
		end
		else begin
			sb <= sa;
			sa <= {enab, disab, eegg};
		end	
	end
	
	assign reset_frames = (sa != sb);

endmodule




//counter to count through the 32 frames of animation and then stop on the last one until a new code is put in
module frames(input logic clk, reset, reset_frames,
	input logic [4:0] counter,
	output logic [4:0] frame);
		
typedef enum logic [4:0] {S0, S1} statetype;
	
	statetype state, nextstate;
	
		always_ff @ (posedge clk)
		begin
		if (reset == 0 | reset_frames) state <= S0;
		else state <= nextstate;
		end
		
	
	
	always_comb
		case (state)
			S0: if (frame == 31) nextstate = S1;
			else nextstate = S0;
			S1: nextstate = S1;
			
			default: nextstate = S0;
		endcase
		
		assign frame = (state == S1) ? 31 : counter;


endmodule



//turn off led sequence
module off_disp(	input logic [4:0] frame,
	output logic [63:0] dot
	);
	
	
	always_comb
		case (frame)
		0: dot = 64'b1111111111111111111111111111111111111111111111111111111111111111;
		
		1: dot = 64'b0000111111111111111111111111111111111111111111111111111111110000;
		
		2: dot = 64'b0000000011111111111111111111111111111111111111111111111100000000;
		
		3: dot = 64'b0000000011111110111111100111111001111110011111110111111100000000;
		
		4: dot = 64'b0000000000011110011111100111111001111110011111100111100000000000;
		
		5: dot = 64'b0000000000000000011111100111111001111110011111100000000000000000;
		
		6: dot = 64'b0000000000000000001111000011110000111100001111000000000000000000;
		
		7: dot = 64'b0000000000000000000000000011110000111100000000000000000000000000;
		
		8: dot = 64'b0000000000000000000000000000000000000000000000000000000000000000;
		
		default: dot = 64'b0000000000000000000000000000000000000000000000000000000000000000;
		
		

endcase
	
	
	
endmodule


//turn on led sequence
module on_disp(	
	input logic [4:0] frame,
	output logic [63:0] dot 
	);


		always_comb
		case (frame)
		0: dot = 64'b1111111100000000000000000000000000000000000000000000000000000000;
					  
		1: dot = 64'b1111111111111111000000000000000000000000000000000000000000000000;
		
		2: dot = 64'b1111111111111111111111110000000000000000000000000000000000000000;
		
		3: dot = 64'b1111111111111111111111111111111100000000000000000000000000000000;
		
		4: dot = 64'b1111111111111111111111111111111111111111000000000000000000000000;
					  
		5: dot = 64'b1111111111111111111111111111111111111111111111110000000000000000;
		
		6: dot = 64'b1111111111111111111111111111111111111111111111111111111100000000;
					  
		7: dot = 64'b1111111111111111111111111111111111111111111111111111111111111111;
					  
		8: dot = 64'b1000000010000000100000001000000010000000100000001000000010000000; 
					 
		9: dot = 64'b1100000011000000110000001100000011000000110000001100000011000000; 
		
		10: dot = 64'b1110000011100000111000001110000011100000111000001110000011100000; 
		
		11: dot = 64'b1111000011110000111100001111000011110000111100001111000011110000; 
		
		12: dot = 64'b1111100011111000111110001111100011111000111110001111100011111000; 
		
		13: dot = 64'b1111110011111100111111001111110011111100111111001111110011111100; 
		
		14: dot = 64'b1111111011111110111111101111111011111110111111101111111011111110; 
		
		15: dot = 64'b1111111111111111111111111111111111111111111111111111111111111111; 
		
		default: dot=64'b1111111111111111111111111111111111111111111111111111111111111111; 
		
		
		
	endcase


endmodule


//time multiplexer to display more complex patterns
module led_tmux(input logic clk, reset,
	input logic [2:0] counter,
	input logic [63:0] dot,
	output logic [7:0] row, colo);
	
   logic [7:0] col;
   
   //flip through rows while displaying corresponding columns
   always_comb
		case (counter)
		0: begin 
			row = ~8'b00000001;
			col = ~dot[63:56];
			end
			
		1: begin 
			row = ~8'b00000010;
			col = ~dot[55:48];
			end
			
		2: begin 
			row = ~8'b00000100;
			col = ~dot[47:40];
			end
			
		3: begin 
			row = ~8'b00001000;
			col = ~dot[39:32];
			end
			
		4: begin 
			row = ~8'b00010000;
			col = ~dot[31:24];
			end
			
		5: begin 
			row = ~8'b00100000;
			col = ~dot[23:16];
			end
			
		6: begin 
			row = ~8'b01000000;
			col = ~dot[15:8];
			end
			
		7: begin 
			row = ~8'b10000000;
			col = ~dot[7:0];
			end
  
  
  endcase
  assign colo = {col[0], col[1], col[2], col[3], col[4], col[5], col[6], col[7]};

  
endmodule



//animation for the easter egg code
module easteregg_disp(	input logic [4:0] frame,
	output logic [63:0] dot
	);


		always_comb
		case (frame)
		0:  dot = 64'b0000000000000000000000000000000000000000000000000000000010000000;	
		
		1: dot = 64'b0000000000000000000000000000000000000000000000001000000001000000;
			
		2: dot = 64'b0000000000000000000000000000000000000000100000000100000000100000;
			
		3: dot = 64'b0000000000000000000000000000000010000000010000000010000000010000;
					  
		4: dot = 64'b0000000000000000000000001000000001000000001000000001000000001000;
			
		5: dot = 64'b0000000000000000100000000100000000100000000100000000100000000100;
			
		6: dot = 64'b0000000010000000010000000010000000010000000010000000010000000010;
		
		7:dot = 64'b1000000001000000001000000001000000001000000001000000001000000001;
			
		8: dot = 64'b0100000000100000000100000000100000000100000000100000000100000000;
			
		9: dot = 64'b0010000000010000000010000000010000000010000000010000000000000000;
			
		10: dot = 64'b0001000000001000000001000000001000000001000000000000000000000000;
		
		11: dot = 64'b0000100000000100000000100000000100000000000000000000000000000000;
			
		12: dot = 64'b0000010000000010000000010000000000000000000000000000000000000000;
			
		13: dot = 64'b0000001000000001000000000000000000000000000000000000000000000000;
		
		14: dot = 64'b0000000100000000000000000000000000000000000000000000000000000000;
			
		15: dot = 64'b0000000000000000000000000000000000000000000000000000000000000000;
			
		16: dot = 64'b0000000000000000000000000000000000000000000000000000000000000000;
		
		17: dot = 64'b0100001010100101111111111001100111111111011111100110011000011000;
		
		18: dot = 64'b0100001010100101111111111001100111111111011111100110011000011000;
		
		19: dot = 64'b0100001010100101111111111001100111111111011111100110011000011000;
		
		20: dot = 64'b0100001010100101111111111001100111111111011111100110011000011000;

		21: dot = 64'b0000010001000110011011100110111101111111111111111111111111111111;
					  
		22: dot = 64'b0000000000000100011011100110111101111111111111111111111111111111;

		23: dot = 64'b0000000000000000000011000110111001111111111111111111111111111111;
		
		24: dot = 64'b0000000000000100011011100110111101111111111111111111111111111111;
		
		25: dot = 64'b0000010001000110011011100110111101111111111111111111111111111111;
					  
		26: dot = 64'b0000000000000100011011100110111101111111111111111111111111111111;

		27: dot = 64'b0000000000000000000011000110111001111111111111111111111111111111;
		
		28: dot = 64'b0100011001100111011011111111111111111111111111111111111111111111;
		
		29: dot = 64'b0100011011111111111111111111111111111111111111111111111111111111;
					    
		30: dot = 64'b1111111111111111111111111111111111111111111111111111111111111111;
		
		default: dot = 64'b1111111111111111111111111111111111111111111111111111111111111111;
			
			
	endcase


endmodule
	
	
	
	
