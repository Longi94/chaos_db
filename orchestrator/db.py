from typing import Set, List, Optional
from sqlalchemy import create_engine, Column, Integer, String, Float, Boolean, BLOB
from sqlalchemy.orm import sessionmaker, scoped_session
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm.exc import NoResultFound, MultipleResultsFound

Base = declarative_base()

DB_SQLITE = 'sqlite'
DB_MONETDB = 'monetdb'
DB_DUCKDB = 'duckdb'
AHEAD_NORMAL = 'ahead_normal'
AHEAD_CONT = 'ahead_cont'
AHEAD_CONT_REENC = 'ahead_cont_reenc'
AHEAD_LATE = 'ahead_late'
AHEAD_EARLY = 'ahead_early'


class Result(Base):
    __tablename__ = 'result'
    iteration = Column(Integer, primary_key=True, nullable=False)
    hostname = Column(String, primary_key=True, nullable=False)
    result = Column(Integer)
    exited = Column(Integer)
    return_code = Column(Integer)
    signaled = Column(Integer)
    term_sig = Column(Integer)
    timeout = Column(Boolean)
    runtime = Column(Float)
    fault_count = Column(Integer)
    max_heap_size = Column(Integer)
    max_stack_size = Column(Integer)
    stdout = Column(BLOB)
    stderr = Column(BLOB)


class ResultsDatabase(object):

    def __init__(self, file):
        self.engine = create_engine(f'sqlite:///{file}')
        Base.metadata.create_all(self.engine)
        self.Session = scoped_session(sessionmaker(bind=self.engine))

    def get_iterations(self) -> Set[int]:
        session = self.Session()
        return set(session.query(Result.iteration))

    def get_iteration(self, iteration: int) -> Optional[Result]:
        session = self.Session()

        try:
            return session.query(Result).filter(Result.iteration == iteration).one()
        except (NoResultFound, MultipleResultsFound):
            return None

    def get_results(self) -> List[Result]:
        session = self.Session()
        return session.query(Result)

    def close(self):
        self.Session().close()
        self.engine.dispose()

    def commit(self):
        self.Session().commit()
