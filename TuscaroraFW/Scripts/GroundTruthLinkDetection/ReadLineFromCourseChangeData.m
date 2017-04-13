% This scripts reads the next line from the the CourseChangeNotifier
% output. The input info can be both MAT info and text info. Depending on
% the variables IsInputDataInTextFormat, the appropriate action is taken
% and internal variables lastreadlinenumber, fileh are kept. 
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
if(IsInputDataInTextFormat)
    if ~exist('lastreadlinenumber', 'var') || ~exist('fileh','var') || ~fileh 
        fileh = fopen(nameofCourseChangeDataFile);
        if(~fileh), printf('Cannot read InitialNodeLocations'); end
        lastreadlinenumber = 0;
    end
    readdata = ReadLineFromCourseChangeDataInText( fileh );
    if numel(readdata)<6 % End of the file is reached or cannot read data
        fclose(fileh);
    end
    lastreadlinenumber = lastreadlinenumber +1 ;
else
    if ~exist('lastreadlinenumber', 'var')
        load('GeneratedMobility.mat', 'CourseChangeDataMatrix');
        lastreadlinenumber = 0;
    end
    if(lastreadlinenumber >= size(CourseChangeDataMatrix,1) )
        readdata = [];
    else
        lastreadlinenumber = lastreadlinenumber + 1;
        readdata = CourseChangeDataMatrix(lastreadlinenumber,:);
    end
end