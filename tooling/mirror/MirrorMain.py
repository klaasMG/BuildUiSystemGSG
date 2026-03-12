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


def get_project_root(project: str) -> str:
    return mirror_projects[project]["project_root"]


def copy_project():
    for project in mirror_projects:
        proj_root_path = resolve_back(mirror_projects[project]["project_root"])

        mirror_repo_base_use = Path(mirror_repo_base) / Path(project)
        mirror_repo_base_use_src = Path(mirror_repo_base_use) / Path(project)

        if mirror_repo_base_use_src.exists():
            shutil.rmtree(mirror_repo_base_use_src)

        shutil.copytree(proj_root_path, mirror_repo_base_use_src)

        if not os.path.isdir(os.path.join(mirror_repo_base_use, ".git")):
            Repo.init(mirror_repo_base_use)

        git_user = github_client.get_user()

        # get repo if it exists, otherwise create it
        try:
            repo_remote = git_user.get_repo(project)
        except:
            repo_remote = git_user.create_repo(project)

        url = repo_remote.clone_url.replace(
            "https://",
            f"https://{GITHUB_TOKEN}@"
        )

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


def main():
    copy_project()


if __name__ == "__main__":
    mirror_repo_base = Path(__file__).parent.parent.parent
    mirror_repo_base = Path(mirror_repo_base) / Path("public_mirror_repository's")
    
    json_file = open("mirror.json", "r")
    mirror_repo = json.load(json_file)
    mirror_projects = mirror_repo["projects"]
    main()