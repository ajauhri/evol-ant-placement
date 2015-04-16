clear all;
tc_id = '1';
filename = strcat('/home/ajauhri/evol-ant-placement/tc',tc_id,'.csv');
delimiter = ',';

formatSpec = '%f%f%f%[^\n\r]';

fileID = fopen(filename,'r');

dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'EmptyValue' ,NaN, 'ReturnOnError', false);

%% Close the text file.
fclose(fileID);

%% Allocate imported array to column variable names
X = dataArray{:, 1};
Y = dataArray{:, 2};
Z = dataArray{:, 3};

%% Clear temporary variables
clearvars filename delimiter formatSpec fileID dataArray ans;
foo = fit([X,Y],Z, 'cubicinterp');
[m,i] = min(Z);
plot(foo, 'Style', 'Contour'); hold on;
colorbar;
plot(X(i),Y(i),'s', 'MarkerSize',12,'MarkerFaceColor', 'white', 'MarkerEdgeColor','red');
plot(X(i),Y(i),'*', 'MarkerSize',12,'MarkerFaceColor', 'white', 'MarkerEdgeColor','red');
hold on;
%hold off;
if tc_id == '1'
    xlabel('Allowable placements for antenna 1');
    ylabel('Allowable placements for antenna 2');
end
if tc_id == '2'
    xlabel('Allowable placements for antennas 2,3');
    ylabel('Allowable placements for antenna 1');
end
if tc_id == '3'
    xlabel('Allowable placements for antennas 1,2');
    ylabel('Allowable placements for antenna 3');
end
if tc_id == '4'
    xlabel('Allowable placements for antennas 1,2,3');
    ylabel('Allowable placements for antenna 4');
end
if tc_id == '5'
    xlabel('Allowable placements for antennas 1,2,3,4,5,6,7,8');
    ylabel('Allowable placements for antenna 9,10');
end
saveas(gca, strcat('/home/ajauhri/quals/paper/FIG/tc',tc_id,'_contour'), 'epsc');
%writerObj = VideoWriter('/home/ajauhri/quals/pres/animation/ga.avi');
%writerObj.FrameRate = 2;
%open(writerObj);
%for i=[1:1:9]
    filename = '/home/ajauhri/evol-ant-placement/run0';
    %filename = strcat(filename, int2str(i));
    if exist(filename, 'file') ~= 2
        break;
    end
    delimiter = ',';
    formatSpec = '%f%f%[^\n\r]';
    fileID = fopen(filename,'r');
    dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter,  'ReturnOnError', false);
    fclose(fileID);
    A = dataArray{:, 1};
    B = dataArray{:, 2};
    pts = plot(A,B,'o','MarkerSize',6,'MarkerFaceColor','black', 'MarkerEdgeColor','black');
    %frame = getframe;
    %writeVideo(writerObj, frame);
    delete(pts)
%end
%close(writerObj);
pts = plot(A,B,'o','MarkerSize',6,'MarkerFaceColor','black', 'MarkerEdgeColor','black');
saveas(gca, strcat('/home/ajauhri/quals/paper/FIG/tc',tc_id,'_hc_anim'), 'epsc');
