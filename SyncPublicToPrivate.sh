git checkout PublicMaster
git pull 
git checkout -b SyncPublictoPrivate
git rebase --onto PublicRelease SyncPoint_TuscaroraPrivate SyncPublictoPrivate
git checkout PublicRelease 
git merge SyncPublictoPrivate
git checkout PublicMaster
git tag -d SyncPoint_TuscaroraPrivate
git tag SyncPoint_TuscaroraPrivate
git branch -D SyncPublictoPrivate
git checkout PublicRelease 
git push
