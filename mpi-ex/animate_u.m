function animate_u(h5file)
% function to animate the results from a wave simulation.
% Call this function with the results file as argument.

u=h5read(h5file, '/u');
u0=squeeze(u(1,:,:));

fig=surf(u0, 'EdgeColor', 'none');
zlim([-1 1])

for i=2:size(u,1)
  set(fig,'ZData', squeeze(u(i,:,:)) );
  drawnow();
  pause(0.25);
end

end

