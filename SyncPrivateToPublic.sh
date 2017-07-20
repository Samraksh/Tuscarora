git checkout PublicMaster
git pull 
git checkout PublicRelease
git fetch --tags
git pull
git checkout -b SyncPrivatetoPublic
git rebase --onto PublicMaster SyncPoint_TuscaroraPublic SyncPrivatetoPublic
git checkout PublicMaster
git merge SyncPrivatetoPublic
git push public HEAD:master
git checkout PublicRelease
git tag -d SyncPoint_TuscaroraPublic
git tag SyncPoint_TuscaroraPublic
git push -f origin SyncPoint_TuscaroraPublic
git branch -D SyncPrivatetoPublic
