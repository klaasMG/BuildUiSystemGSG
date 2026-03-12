import os
import json
import shutil
from pathlib import Path
from git import Repo
from dotenv import load_dotenv
from github import Github, Auth

load_dotenv()
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
github_client = Github(auth=Auth.Token(GITHUB_TOKEN))

base_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(base_dir)


def resolve_back(path_str: str, base: Path = Path(__file__).parent) -> Path:
    path = base
    for part in path_str.split("/"):
        if part == "..":
            path = path.parent
        elif part and part != ".":
            path = path / part
    return path.resolve()

def set_remote_repo(git_user, project, private: bool):
    # get repo if it exists, otherwise create it
    try:
        repo_remote = git_user.get_repo(project)
    except:
        repo_remote = git_user.create_repo(project, private=private)
    
    url = repo_remote.clone_url.replace(
        "https://",
        f"https://{GITHUB_TOKEN}@"
    )
    return url

def push_to_repo(url, mirror_repo_base_use):
    repo = Repo(mirror_repo_base_use)
    
    if repo.is_dirty(untracked_files=True):
        repo.git.add(".")
        repo.index.commit("commit")
    
    # ensure remote exists
    if "origin" not in [r.name for r in repo.remotes]:
        origin = repo.create_remote("origin", url)
    else:
        origin = repo.remotes.origin
        origin.set_url(url)
    
    branch = repo.active_branch.name
    origin.push(refspec=f"{branch}:{branch}")

def copy_project():
    for project in mirror_projects:
        proj_root_path = resolve_back(mirror_projects[project]["project_root"])
        proj_root_path = Path(proj_root_path)
        proj_root_path = mirror_base / proj_root_path.name
        
        mirror_repo_base_use = Path(mirror_repo_base) / Path(project)
        mirror_repo_base_use_src = Path(mirror_repo_base_use) / Path(project)

        if mirror_repo_base_use_src.exists():
            shutil.rmtree(mirror_repo_base_use_src)

        shutil.copytree(proj_root_path, mirror_repo_base_use_src)

        if not os.path.isdir(os.path.join(mirror_repo_base_use, ".git")):
            Repo.init(mirror_repo_base_use)

        git_user = github_client.get_user()
        
        is_public = mirror_projects[project]["repo-public"]

        url = set_remote_repo(git_user, project ,is_public)
        
        push_to_repo(url, mirror_repo_base_use)

def main():
    copy_project()

if __name__ == "__main__":
    mirror_repo_base = Path(__file__).parent.parent.parent
    mirror_repo_base = Path(mirror_repo_base) / Path("public_mirror_repository's")
    json_file = open("mirror.json", "r")
    mirror_repo = json.load(json_file)
    mirror_projects = mirror_repo["projects"]
    mirror_base = mirror_repo["base"]
    mirror_base = resolve_back(mirror_base, Path(__file__).parent)
    mirror_base = Path(mirror_base)
    main()