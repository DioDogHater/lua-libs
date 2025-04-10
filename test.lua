SDL.init()

win = SDL.create_window("Test window",{w=500,h=500});

print(win)

while win:is_active() do

	for event in SDL.poll_events() do
		if event.type == SDL.QUIT then
			win:close()
		elseif event.type == SDL.KEYDOWN then
			if event.key == SDL.key.ESCAPE then
				win:close()
			end
		end
	end

	win:clear({r=25,g=75,b=175})

	win:draw_rect({x=10,y=10,w=50,h=50}, {r=0,g=0,b=0})
	win:draw_line({x=0,y=0},{x=100,y=100},SDL.color(255))

	win:update()
end

SDL.quit()