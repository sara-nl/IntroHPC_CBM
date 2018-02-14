function animate2_u(h5file)
% Animation of the surface of the wave simulation in this directory.
% This is a very nice animation using lighting; it requires matlab 2014b
% or later.

u=h5read(h5file, '/u');
u0=squeeze(u(1,:,:));

fig=surf(u0, 'EdgeColor', 'none');

caxis([-1 1]);
zlim([-1 1]);
daspect([max(daspect)*[1 1] 1]);

shading interp;
lightangle(-45,30);
fig.FaceLighting = 'gouraud';
fig.AmbientStrength = 0.3;
fig.DiffuseStrength = 0.8;
fig.SpecularStrength = 0.9;
fig.SpecularExponent = 25;
fig.BackFaceLighting = 'unlit';


for i=2:size(u,1)
  set(fig,'ZData', squeeze(u(i,:,:)) );
  drawnow();
  pause(0.25);
end

end
