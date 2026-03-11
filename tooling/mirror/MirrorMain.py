import os
import json
import shutil
from pathlib import Path
from git import Repo

mirror_repo_base = Path(__file__).parent.parent.parent
mirror_repo_base = Path(mirror_repo_base) / Path("public_mirror_repository's")

json_file = open("tooling/mirror/mirror.json", "r")
mirror_repo = json.load(json_file)
mirror_projects = mirror_repo["projects"]

def get_project_root(project: str)-> str:
    return mirror_projects[project]["project_root"]

def copy_project():
    for project in mirror_projects:
        proj_root = get_project_root(project)
        proj_root_path = Path(proj_root)
        proj_root_path = proj_root_path.absolute()
        mirror_repo_base_use = Path(mirror_repo_base) / Path(f"{project}")
        mirror_repo_base_use_src = Path(mirror_repo_base_use) / Path(f"{project}")
        if mirror_repo_base_use_src.exists():
            shutil.rmtree(mirror_repo_base_use_src)
        shutil.copytree(proj_root_path, mirror_repo_base_use_src)
        if not os.path.isdir(os.path.join(mirror_repo_base_use, ".git")):
            Repo.init(mirror_repo_base_use)
        repo = Repo(mirror_repo_base_use)
        repo.git.add(".")
        repo.index.commit("commit")
        
    
def main():
    copy_project()

if __name__ == "__main__":
    main()