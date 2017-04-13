function outputdata = ReadLineFromCourseChangeDataInText( fh )
%ReadLineFromCourseChangeDataInText This function reads the next line from the the CourseChangeNotifier
%   Detailed explanation goes here
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
    if feof(fh), outputdata = -1; 
    else
        out = textscan(fh, '%*[^=]%*c %f %s %*[^=]%*c %f %*[^=]%*c %f %*[^=]%*c %f %*[^=]%*c %f %*[^=]%*c %f ', 1);
        %CourseChange at t = 0.358509 /NodeList/17/$ns3::MobilityModel/CourseChange Vx=3.49699, Vy=1.17763 x=19.4282, y=16.8204, z=0
        if size(out,2) ~= 7 ||   numel(out{1}) == 0
            outputdata = -1;  
        else   
            t = out{1};
            NodeID = sscanf( out{2}{1}, '%*c%*[^/]%*c%f%*s'  );
            Vx = out{3};
            Vy = out{4};
            positionx =  out{5};
            positiony = out{6};
            outputdata = [t, NodeID, Vx, Vy, positionx, positiony];
        end
    end
end
